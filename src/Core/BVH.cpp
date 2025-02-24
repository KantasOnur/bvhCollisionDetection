#include "BVH.h"
#include "EntityManager.h"
#include <iostream>
#include <algorithm>
#include <bit>

bool comp(LeafNode i, LeafNode j)
{
	return i.code < j.code;
}


BVH::BVH::BVH(const unsigned int& entityID)
	: m_entityID(entityID)
{
	std::vector <LeafNode> mortonCodes = _getMortonCodes();
	std::sort(mortonCodes.begin(), mortonCodes.end(), comp);

	m_hierarchy = _generateHierarchy(mortonCodes, 0, mortonCodes.size() - 1);
	
	std::vector<LeafNode> leafNodes(mortonCodes.size());
	std::vector<InternalNode> internalNodes(mortonCodes.size() - 1);
	for (int i = 0; i < mortonCodes.size(); ++i)
	{
		leafNodes[i] = mortonCodes[i];
	}
	for (int i = 0; i < mortonCodes.size() - 1; ++i)
	{
		glm::vec2 range = _determineRnage(mortonCodes, i);
		int first = range.x;
		int last = range.y;


		int split = _findSplit(mortonCodes, first, last);

		unsigned int left = split;
		if (split == first)
			internalNodes[i].is_left_internal = 0;

		unsigned int right = split + 1;
		if (split + 1 == last)
			internalNodes[i].is_right_internal = 0;


		internalNodes[i].left = left;
		internalNodes[i].right = right;


		if (internalNodes[i].is_left_internal)
			internalNodes[left].parent = i;
		else
			leafNodes[left].parent = i;

		if (internalNodes[i].is_right_internal)
			internalNodes[right].parent = i;
		else
			leafNodes[right].parent = i;

		internalNodes[i].start = first;
		internalNodes[i].end = last;
	}
	/*
	int c = 0;
	for (const auto& asd : internalNodes)
	{
		if (asd.parent == 0)
		{
 			std::cout << "here" << std::endl;
		}
		c++;
	}
	*/
	GLBuffer<LeafNode> ln = GLBuffer<LeafNode>(GL_SHADER_STORAGE_BUFFER, leafNodes, GL_STATIC_DRAW);
	GLBuffer<InternalNode> in = GLBuffer<InternalNode>(GL_SHADER_STORAGE_BUFFER, internalNodes, GL_STATIC_DRAW);

	in.sendToGPU(0);
	ln.sendToGPU(1);

	
	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + leafNodes.size() - 1) / threadsPerBlock;

	_computeBounds.bind();
	_computeBounds.setUInt("numLeafs", leafNodes.size());
	_computeBounds.unbind();

	_computeBounds.dispatch(numBlocks, 1, 1);

	std::vector<InternalNode> rip = in.retrieveBuffer();
	std::vector<LeafNode> rip2 = ln.retrieveBuffer();

	for (auto& rip3 : leafNodes)
	{
		if (rip3.parent == -1)
		{
			std::cout << "here idk" << std::endl;
		}
	}

	InternalNode rip4 = rip[4049];
	InternalNode rip5 = rip[4050];

	glm::vec3 min, max;
	EntityManager::getInstance().getEntity(entityID).getMinMax(min, max);
	std::cout << "here" << std::endl;
}


void BVH::BVH::_destroyRecursive(Node* node)
{
	if (!node) return;

	_destroyRecursive(node->left);
	_destroyRecursive(node->right);
	delete node;
	node = nullptr;
}

BVH::BVH::~BVH()
{
	_destroyRecursive(m_hierarchy);

}

std::vector<LeafNode> BVH::BVH::_getMortonCodes()
{
	glm::vec3 min, max;
	Entity& entity = EntityManager::getInstance().getEntity(m_entityID);
	const GLBuffer<MeshLoader::Vertex>& verticies = entity.getVerticies();
	const GLBuffer<unsigned int>& indicies = entity.getIndicies();
	entity.getMinMax(min, max);


	const unsigned int triangleCount = indicies.getSize() / 3;
	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + triangleCount - 1) / threadsPerBlock;

	m_mortonCodes = std::make_unique<GLBuffer<LeafNode>>(GL_SHADER_STORAGE_BUFFER, nullptr, triangleCount, GL_STATIC_DRAW);

	m_mortonCodes->sendToGPU(0);
	verticies.sendToGPU(1);
	indicies.sendToGPU(2);

	_computeMortonCodes.bind();
	_computeMortonCodes.setVec3f("u_min", min);
	_computeMortonCodes.setVec3f("u_max", max);
	_computeMortonCodes.setUInt("triangleCount", triangleCount);
	_computeMortonCodes.unbind();

	_computeMortonCodes.dispatch(numBlocks, 1, 1);

	return m_mortonCodes->retrieveBuffer();
}

BVH::Node* BVH::BVH::_generateHierarchy(const std::vector<LeafNode>& codes,
										const unsigned int&	first, 
										const unsigned int&	last)
{
	if (first == last) return new Node{ first, last, nullptr, nullptr };

	int split = _findSplit(codes, first, last);

	Node* childA = _generateHierarchy(codes, first, split);
	Node* childB = _generateHierarchy(codes, split + 1, last);

	return new Node{ first, last, childA, childB };
}

int BVH::BVH::_findSplit(const std::vector<LeafNode>& codes,
						 const unsigned int& first,
						 const unsigned int& last)
{
	unsigned int firstCode = codes[first].code;
	unsigned int lastCode = codes[last].code;

	if (firstCode == lastCode) return first;//(first + last) >> 1;

	int commonPrefix = std::countl_zero(firstCode ^ lastCode);

	int split = first;
	int step = last - first;

	do
	{
		step = (step + 1) >> 1;
		int newSplit = split + step;
		if (newSplit < last)
		{
			unsigned int splitCode = codes[newSplit].code;
			int splitPrefix = std::countl_zero(firstCode ^ splitCode);
			if (splitPrefix > commonPrefix)
				split = newSplit;
		}
	} while (step > 1);
	return split;
}

int delta(unsigned int a, unsigned int b)
{
	return std::countl_zero(a ^ b);
}


int BVH::BVH::_countCommonPrefix(const std::vector<LeafNode> codes,
	int i, int j)
{
	if (i >= codes.size() || j >= codes.size()) return -1;
	if (i < 0 || j < 0) return -1;

	unsigned int codeA = codes[i].code;
	unsigned int codeB = codes[j].code;

	if (codeA == codeB)
		return 16 + std::countl_zero((unsigned int)i ^ (unsigned int)j);

	return std::countl_zero(codeA ^ codeB);
}

glm::vec2 BVH::BVH::_determineRnage(const std::vector<LeafNode> mortonCode, const int& i)
{
	// If both neighbors exist and are equal to the current code, then simply scan right
	if (i > 0 && i < mortonCode.size() - 1 &&
		mortonCode[i - 1].code == mortonCode[i].code &&
		mortonCode[i].code == mortonCode[i + 1].code)
	{
		int j = i;
		// Walk to the right until a difference is found or we hit the end
		while (j < mortonCode.size() - 1 && mortonCode[j].code == mortonCode[j + 1].code)
		{
			++j;
		}
		return glm::vec2(glm::min(i, j), glm::max(i, j));
	}

	// Otherwise, proceed with the binary search as before.
	int d = _countCommonPrefix(mortonCode, i, i + 1) - _countCommonPrefix(mortonCode, i, i - 1);
	d = d < 0 ? -1 : 1;

	int deltaMin = _countCommonPrefix(mortonCode, i, i - d);
	int l_max = 2;
	while (_countCommonPrefix(mortonCode, i, i + l_max * d) > deltaMin)
	{
		l_max *= 2;
	}
	int l = 0;
	for (int t = l_max >> 1; t >= 1; t >>= 1)
	{
		if (_countCommonPrefix(mortonCode, i, i + (l + t) * d) > deltaMin)
		{
			l += t;
		}
	}
	int j = i + l * d;
	return glm::vec2(glm::min(i, j), glm::max(i, j));
}
