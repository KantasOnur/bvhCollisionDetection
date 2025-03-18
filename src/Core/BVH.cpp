#include "BVH.h"
#include "EntityManager.h"
#include <iostream>
#include <algorithm>
#include <bit>
#include <random>
#include "Gui.h"
#include <format>
#include <chrono>
#include "RadixSort.h"

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
	/*
	_constructLeafNodes();
	_constructInternalNodes();
	_constructBounds();
	*/


	int n = pow(2,20);
	std::mt19937 gen;
	std::uniform_int_distribution<unsigned int> dist(0, -1);
	std::vector<unsigned int> values(n); //= {7, 14, 4, 1};

	//std::cout << "values: " << std::endl;
	for (auto& value : values) value = dist(gen);
	GLBuffer<unsigned int> data_in(GL_SHADER_STORAGE_BUFFER, values, GL_STATIC_DRAW);
	//GLBuffer<unsigned int> data_out(GL_SHADER_STORAGE_BUFFER, values, GL_STATIC_DRAW);
	//_radixSort(data_in, data_out);
	RadixSort r(data_in);
	r.sort();

}

void BVH::BVH::_buildHistogram(GLBuffer<unsigned int>& data_in, 
							   GLBuffer<unsigned int>& histogram,
							   const unsigned int& bitStage)
{
	const unsigned int n = data_in.getSize();
	histogram.sendToGPU(0);
	data_in.sendToGPU(1);

	const unsigned int numThreads = 1024;
	const unsigned int numBlocks = (numThreads + n - 1) / numThreads;

	_computeHistogram.bind();
	_computeHistogram.setUInt("n", n);
	_computeHistogram.setUInt("bitStage", bitStage);
	_computeHistogram.unbind();

	_computeHistogram.dispatch(numBlocks, 1, 1);
}

void BVH::BVH::_prefixSum(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out)
{
	/*
	unsigned int n = data_in.getSize();
	auto values = data_in.retrieveBuffer();
	std::vector<unsigned int> values_out(n);
	values_out[0] = 0;
	for (int i = 1; i < n; ++i)
	{
		values_out[i] = values[i - 1] + values_out[i - 1];
	}
	
	unsigned int numThreads = 512;
	unsigned int numBlocks = (numThreads + (n / 2) - 1) / numThreads;


	GLBuffer<unsigned int> sums(GL_SHADER_STORAGE_BUFFER, nullptr, numBlocks, GL_STATIC_DRAW);
	GLBuffer<unsigned int> incr(GL_SHADER_STORAGE_BUFFER, nullptr, numBlocks, GL_STATIC_DRAW);

	data_in.sendToGPU(0);
	data_out.sendToGPU(1);
	sums.sendToGPU(2);
	_computePrefixSum.bind();
	_computePrefixSum.setInt("n", n);
	_computePrefixSum.unbind();
	_computePrefixSum.dispatch(numBlocks, 1, 1);

	
	sums.sendToGPU(0);
	incr.sendToGPU(1);
	_computePrefixSumAux.bind();
	_computePrefixSumAux.setInt("n", numBlocks);
	_computePrefixSumAux.unbind();
	_computePrefixSumAux.dispatch(1, 1, 1);

	numThreads = 1024;
	numBlocks = (numThreads + n - 1) / numThreads;
	incr.sendToGPU(0);
	data_out.sendToGPU(1);
	_computeUniformIncrement.bind();
	_computeUniformIncrement.setInt("n", n);
	_computeUniformIncrement.unbind();
	_computeUniformIncrement.dispatch(numBlocks, 1, 1);

	std::vector<unsigned int> prefixSum = data_out.retrieveBuffer();
	std::vector<unsigned int> sums_host = sums.retrieveBuffer();
	std::vector<unsigned int> incr_host = incr.retrieveBuffer();

	for (int i = 0; i < n; ++i)
	{
		if (values_out[i] != prefixSum[i])
		{
			std::cout << "wwhy???" << std::endl;
		}
	}
	*/
	unsigned int n = data_in.getSize();
	unsigned int numThreads = 512;
	unsigned int numBlocks = (numThreads + (n / 2) - 1) / numThreads;

	if (numBlocks == 1) return _prefixSumBlock(data_in, data_out);

	GLBuffer<unsigned int> sums(GL_SHADER_STORAGE_BUFFER, nullptr, numBlocks, GL_STATIC_DRAW);
	GLBuffer<unsigned int> incr(GL_SHADER_STORAGE_BUFFER, nullptr, numBlocks, GL_STATIC_DRAW);

	data_in.sendToGPU(0);
	data_out.sendToGPU(1);
	sums.sendToGPU(2);
	_computePrefixSum.bind();
	_computePrefixSum.setInt("n", n);
	_computePrefixSum.unbind();
	_computePrefixSum.dispatch(numBlocks, 1, 1);

	_prefixSumBlock(sums, incr);

	numThreads = 1024;
	numBlocks = (numThreads + n - 1) / numThreads;
	incr.sendToGPU(0);
	data_out.sendToGPU(1);
	_computeUniformIncrement.bind();
	_computeUniformIncrement.setInt("n", n);
	_computeUniformIncrement.unbind();
	_computeUniformIncrement.dispatch(numBlocks, 1, 1);

}
void BVH::BVH::_prefixSumBlock(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out)
{
	data_in.sendToGPU(0);
	data_out.sendToGPU(1);
	_computePrefixSumAux.bind();
	_computePrefixSumAux.setInt("n", data_in.getSize());
	_computePrefixSumAux.unbind();
	_computePrefixSumAux.dispatch(1, 1, 1);

}
void BVH::BVH::_setBuffers(GLBuffer<unsigned int>& data_in,
						   GLBuffer<unsigned int>& setBits,
						   GLBuffer<unsigned int>& setBitsPrefixSum,
						   GLBuffer<unsigned int>& unsetBits,
						   GLBuffer<unsigned int>& unsetBitsPrefixSum,
						   GLBuffer<unsigned int>& histogram,
						   GLBuffer<unsigned int>& histogramPrefixSum,
						   const unsigned int& bitStage)
{
	const unsigned int numThreads = 1024;
	const unsigned int numBlocks = (numThreads + data_in.getSize() - 1) / numThreads;
	data_in.sendToGPU(0);
	setBits.sendToGPU(1);
	unsetBits.sendToGPU(2);
	setBitsPrefixSum.sendToGPU(3);
	unsetBitsPrefixSum.sendToGPU(4);
	histogram.sendToGPU(5);
	histogramPrefixSum.sendToGPU(6);

	_computeBuildBuffers.bind();
	_computeBuildBuffers.setUInt("bitStage", bitStage);
	_computeBuildBuffers.setUInt("n", data_in.getSize());
	_computeBuildBuffers.unbind();

	_computeBuildBuffers.dispatch(numBlocks, 1, 1);
}

void BVH::BVH::_applyOffsets(GLBuffer<unsigned int>& data_in,
							 GLBuffer<unsigned int>& data_out,
							 GLBuffer<unsigned int>& histogram,
							 GLBuffer<unsigned int>& setBitsPrefixSum,
							 GLBuffer<unsigned int>& unsetBitsPrefixSum,
							 const unsigned int& bitStage)
{
	const unsigned int n = data_in.getSize();
	const unsigned int numThreads = 1024;
	const unsigned int numBlocks = (numThreads + n - 1) / 1024;

	data_in.sendToGPU(0);
	data_out.sendToGPU(1);
	histogram.sendToGPU(2);
	setBitsPrefixSum.sendToGPU(3);
	unsetBitsPrefixSum.sendToGPU(4);

	_computeApplyOffsets.bind();
	_computeApplyOffsets.setUInt("n", n);
	_computeApplyOffsets.setUInt("bitStage", bitStage);
	_computeApplyOffsets.unbind();

	_computeApplyOffsets.dispatch(numBlocks, 1, 1);
}

void BVH::BVH::_copyElements(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out)
{
	int n = data_in.getSize();
	const unsigned int numThreads = 1024;
	const unsigned int numBlocks = (numThreads + n - 1) / numThreads;

	_computeCopyElements.bind();
	_computeCopyElements.setUInt("n", n);
	_computeCopyElements.unbind();

	_computeCopyElements.dispatch(numBlocks, 1, 1);
}

void BVH::BVH::_radixSort(GLBuffer<unsigned int>& data_in, GLBuffer<unsigned int>& data_out)
{
	GLBuffer<unsigned int> setBits
		(GL_SHADER_STORAGE_BUFFER, nullptr, data_in.getSize(), GL_STATIC_DRAW);
	GLBuffer<unsigned int> setBitsPrefixSum
		(GL_SHADER_STORAGE_BUFFER, nullptr, data_in.getSize(), GL_STATIC_DRAW);
	GLBuffer<unsigned int> unsetBits
		(GL_SHADER_STORAGE_BUFFER, nullptr, data_in.getSize(), GL_STATIC_DRAW);
	GLBuffer<unsigned int> unsetBitsPrefixSum
		(GL_SHADER_STORAGE_BUFFER, nullptr, data_in.getSize(), GL_STATIC_DRAW);
	GLBuffer<unsigned int> histogram
		(GL_SHADER_STORAGE_BUFFER, nullptr, 2, GL_STATIC_DRAW);
	GLBuffer<unsigned int> histogramPrefixSum
		(GL_SHADER_STORAGE_BUFFER, nullptr, 2, GL_STATIC_DRAW);

	//std::cout << "begin" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	for (unsigned int bitStage = 0; bitStage < 32; ++bitStage)
	{
		_setBuffers(data_in, setBits, setBitsPrefixSum, unsetBits, unsetBitsPrefixSum, histogram, histogramPrefixSum,bitStage);
		_buildHistogram(data_in, histogram, bitStage);
		_prefixSum(setBits, setBitsPrefixSum);

		_prefixSum(unsetBits, unsetBitsPrefixSum);
		_applyOffsets(data_in, data_out, histogram, setBitsPrefixSum, unsetBitsPrefixSum, bitStage);
		_copyElements(data_in, data_out);
	}

	auto finish = std::chrono::high_resolution_clock::now();

	// Calculate the elapsed time in milliseconds (or pick another unit if preferred)
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

	std::cout << "Elapsed time: " << elapsed << " ms" << std::endl;

	auto a = data_in.retrieveBuffer();
	auto b = data_out.retrieveBuffer();

	for (int i = 0; i < b.size() - 1; ++i)
	{
		if (b[i] > b[i + 1]) std::cout << "not sorted" << std::endl;
	}
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
	m_leafNodes->updateBuffer(leafNodes.data(), leafNodes.size());

	unsigned int min = leafNodes[0].code;
	unsigned int max = leafNodes[leafNodes.size() - 1].code;

	std::cout << min << " " << max << " " << leafNodes.size() << std::endl;
}

void BVH::BVH::_constructInternalNodes()
{
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

	std::vector<LeafNode> leafNodes = m_leafNodes->retrieveBuffer();
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