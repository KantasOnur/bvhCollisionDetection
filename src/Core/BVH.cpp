#include "BVH.h"
#include "EntityManager.h"
#include <iostream>
#include <algorithm>
#include <bit>
#include <random>
#include "Gui.h" x

// Generate random floats for each component
static glm::vec3 getColor(unsigned int nodeID)
{
	std::mt19937 gen(nodeID);
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	float r = dist(gen);
	float g = dist(gen);
	float b = dist(gen);

	return glm::vec3(r, g, b);
}

bool comp(LeafNode i, LeafNode j)
{
	return i.code < j.code;
}


BVH::BVH::BVH(const unsigned int& entityID)
	: m_entityID(entityID), Mesh("cube")
{
	
	_constructLeafNodes();
	_constructInternalNodes();
	_constructBounds();
}

std::vector<InternalNode> BVH::BVH::_constructInternalNodesCPU(std::vector<LeafNode>& leafNodes)
{
	std::vector<InternalNode> internalNodes(leafNodes.size() - 1);

	for (int i = 0; i < internalNodes.size(); ++i)
	{
		//std::cout << i << std::endl;
		glm::vec2 range = _determineRnage(leafNodes, i);
		int first = range.x;
		int last = range.y;


		int split = _findSplit(leafNodes, first, last);

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

	return internalNodes;
}

void BVH::BVH::draw(const Camera& camera)
{
	static int depth = 0;
	ImGui::Begin("BVH");
	ImGui::InputInt("BVH depth", &depth, 1);
	std::vector<InternalNode> nodes = m_internalNodes->retrieveBuffer();
	drawRecursive(camera, depth, nodes, 0);
	ImGui::End();
}

void BVH::BVH::drawRecursive(const Camera& camera, const unsigned int depth, 
	const std::vector<InternalNode>& nodes, const unsigned int currentNode)
{
	if (depth == 0)
	{
		shader.bind();
		shader.setMatrix4f("projectionMatrix", camera.getProjection());
		shader.setMatrix4f("viewMatrix", camera.getView());
		shader.setMatrix4f("modelMatrix", glm::mat4(1.0f));

		shader.setVec4f("min", nodes[currentNode].box.min);
		shader.setVec4f("max", nodes[currentNode].box.max);

		
		shader.setVec3f("colour", getColor(currentNode));
		_drawMesh(camera);

		shader.unbind();
		return;
	}

	if(nodes[currentNode].is_left_internal == 1) drawRecursive(camera, depth - 1, nodes, nodes[currentNode].left);
	if(nodes[currentNode].is_right_internal == 1) drawRecursive(camera, depth - 1, nodes, nodes[currentNode].right);
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


void BVH::BVH::_constructLeafNodes()
{
	std::vector <LeafNode> leafNodes = _getMortonCodes();
	std::sort(leafNodes.begin(), leafNodes.end(), comp);
	//std::cout << leafNodes[leafNodes.size() - 1].code << std::endl;
	m_leafNodes->updateBuffer(leafNodes.data(), leafNodes.size());
}

void BVH::BVH::_constructInternalNodes()
{
	/*
	std::vector<InternalNode> internalNodes(m_leafNodes->getSize() - 1);
	std::vector<LeafNode> leafNodes = m_leafNodes->retrieveBuffer();
	for (int i = 0; i < internalNodes.size(); ++i)
	{
		//std::cout << i << std::endl;
		glm::vec2 range = _determineRnage(leafNodes, i);
		int first = range.x;
		int last = range.y;


		int split = _findSplit(leafNodes, first, last);

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

	m_leafNodes->updateBuffer(leafNodes.data(), leafNodes.size());
	m_internalNodes = std::make_unique<GLBuffer<InternalNode>>(GL_SHADER_STORAGE_BUFFER, internalNodes, GL_DYNAMIC_DRAW);
	*/
	m_internalNodes = std::make_unique<GLBuffer<InternalNode>>(GL_SHADER_STORAGE_BUFFER, std::vector<InternalNode>(m_leafNodes->getSize() - 1), GL_DYNAMIC_DRAW);
  	std::vector<InternalNode> internals = m_internalNodes->retrieveBuffer();

	m_internalNodes->sendToGPU(0);
	m_leafNodes->sendToGPU(1);

	_computeInternalsNodes.bind();
	_computeInternalsNodes.setUInt("numLeaves", m_leafNodes->getSize());
	_computeInternalsNodes.setUInt("numInternals", m_leafNodes->getSize() - 1);
	_computeInternalsNodes.unbind();

	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + m_numInternals - 1) / threadsPerBlock;
	_computeInternalsNodes.dispatch(numBlocks, 1, 1);

	
	/*
	internals = m_internalNodes->retrieveBuffer();
	std::vector<LeafNode> leaves = m_leafNodes->retrieveBuffer();
	int i = 0;
	for (const auto& internal: internals)
	{
		if (internal.parent == -1)
		{
			std::cout << i << std::endl;
			//std::cout << "here" << std::endl;
		}
		i++;
	}
	
	/*
	for (const auto& leaf : leaves)
	{
		if (leaf.pad0 >= 1)
		{
			std::cout << leaf.pad0 << std::endl;
		}
	}
	*/
}

void BVH::BVH::_constructBounds()
{
	
	m_internalNodes->sendToGPU(0);
	m_leafNodes->sendToGPU(1);

	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + m_leafNodes->getSize() - 1) / threadsPerBlock;

	_computeBounds.bind();
	_computeBounds.setUInt("numLeafs", m_leafNodes->getSize());
	_computeBounds.unbind();

	_computeBounds.dispatch(numBlocks, 1, 1);
	


	std::vector<InternalNode> rip = m_internalNodes->retrieveBuffer();

	//glm::vec3 min, max;
	//EntityManager::getInstance().getEntity(m_entityID).getMinMax(min, max);
	//std::cout << "here" << std::endl;
	/*
	for (const auto& internal: rip)
	{
		if (internal.count_arrival != 2)
		{
			std::cout << internal.count_arrival << std::endl;
		}
	}
	*/
	
}

Box BVH::BVH::_constructBoundsCPU(std::vector<LeafNode>& leafNodes,
								  std::vector<InternalNode>& internalNodes,
								  unsigned int currentNode)
{

	Box left, right;

	if (internalNodes[currentNode].is_left_internal == 0)
		left = leafNodes[internalNodes[currentNode].left].box;
	else
		left = _constructBoundsCPU(leafNodes, internalNodes, internalNodes[currentNode].left);
	if (internalNodes[currentNode].is_right_internal == 0)
		right = leafNodes[internalNodes[currentNode].right].box;
	else
		right = _constructBoundsCPU(leafNodes, internalNodes, internalNodes[currentNode].right);

	internalNodes[currentNode].box = Box(glm::min(left.min, right.min), glm::max(left.max, right.max));
	return internalNodes[currentNode].box;
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

	m_leafNodes = std::make_unique<GLBuffer<LeafNode>>(GL_SHADER_STORAGE_BUFFER, nullptr, triangleCount, GL_STATIC_DRAW);

	m_leafNodes->sendToGPU(0);
	verticies.sendToGPU(1);
	indicies.sendToGPU(2);

	_computeMortonCodes.bind();
	_computeMortonCodes.setVec3f("u_min", min);
	_computeMortonCodes.setVec3f("u_max", max);
	_computeMortonCodes.setUInt("triangleCount", triangleCount);
	_computeMortonCodes.unbind();

	_computeMortonCodes.dispatch(numBlocks, 1, 1);

	m_numLeaves = triangleCount;
	m_numInternals = m_numLeaves - 1;
	return m_leafNodes->retrieveBuffer();
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

	//bool eq = false;
	//if (firstCode == lastCode) return (first + last) >> 1;

	int commonPrefix = _countCommonPrefix(codes, first, last);

	int split = first;
	int step = last - first;

	do
	{
		step = (step + 1) >> 1;
		int newSplit = split + step;
		if (newSplit < last)
		{
			//unsigned int splitCode = codes[newSplit].code;
			int splitPrefix = _countCommonPrefix(codes, first, newSplit);//std::countl_zero(firstCode ^ splitCode);
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
		return 32 + std::countl_zero((unsigned int)i ^ (unsigned int)j);

	return std::countl_zero(codeA ^ codeB);
}

glm::vec2 BVH::BVH::_determineRnage(const std::vector<LeafNode> mortonCode, const int& i)
{
	/*
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
	*/
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
