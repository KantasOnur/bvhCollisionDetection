#include "BVH.h"
#include "EntityManager.h"
#include <iostream>
#include <algorithm>
#include <bit>
#include <random>
#include "Gui.h"
#include <format>

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
	std::uniform_int_distribution<unsigned int> dist(0, 10);
	std::vector<unsigned int> values(n);


	std::cout << "values: " << std::endl;
	for (auto& value : values)
	{
		value = dist(gen);
		//std::cout << value << std::endl;
	}
	_prefixSum(values);


}

void BVH::BVH::_buildHistogram(std::vector<unsigned int> values)
{
	std::vector<unsigned int> values0(values.size());
	std::vector<unsigned int> values1(values.size());

	GLBuffer<unsigned int> device_values(GL_SHADER_STORAGE_BUFFER, values, GL_STATIC_DRAW);
	GLBuffer<unsigned int> device_values0(GL_SHADER_STORAGE_BUFFER, values0, GL_STATIC_DRAW);
	GLBuffer<unsigned int> device_values1(GL_SHADER_STORAGE_BUFFER, values1, GL_STATIC_DRAW);

	std::vector<unsigned int> histogram = { 0, 0 };
	GLBuffer<unsigned int> device_histogram(GL_SHADER_STORAGE_BUFFER, histogram, GL_STATIC_DRAW);


	device_histogram.sendToGPU(0);
	device_values.sendToGPU(1);
	device_values0.sendToGPU(2);
	device_values1.sendToGPU(3);


	const unsigned int numThreads = 1024;
	const unsigned int numBlocks = (numThreads + values.size() - 1) / numThreads;

	for (int bitStage = 0; bitStage < 4; ++bitStage)
	{
		_computeHistogram.bind();
		_computeHistogram.setUInt("numUints", values.size());
		_computeHistogram.setUInt("bitStage", bitStage);
		_computeHistogram.unbind();
		_computeHistogram.dispatch(numBlocks, 1, 1);

		histogram = device_histogram.retrieveBuffer();

		std::cout << "bitStage: " << bitStage << std::endl;
		std::cout << "histogram[0]: " << histogram[0] << std::endl;
		std::cout << "histogram[1]: " << histogram[1] << std::endl;

		histogram = { 0, 0 };
		device_histogram.updateBuffer(histogram.data(), histogram.size());
	}
	values = device_values.retrieveBuffer();
}

void BVH::BVH::_prefixSum(std::vector<unsigned int> values)
{
	std::vector<unsigned int> values_out(values.size());
	values_out[0] = 0;
	for (int i = 1; i < values.size(); ++i)
	{
		values_out[i] = values[i - 1] + values_out[i - 1];
	}

	unsigned int numThreads = 512;
	unsigned int numBlocks = (numThreads + (values.size() / 2) - 1) / numThreads;


	GLBuffer<unsigned int> data_in(GL_SHADER_STORAGE_BUFFER, values, GL_STATIC_DRAW);
	GLBuffer<unsigned int> data_out(GL_SHADER_STORAGE_BUFFER, nullptr, values.size(), GL_STATIC_DRAW);
	GLBuffer<unsigned int> sums(GL_SHADER_STORAGE_BUFFER, nullptr, numBlocks, GL_STATIC_DRAW);
	GLBuffer<unsigned int> incr(GL_SHADER_STORAGE_BUFFER, nullptr, numBlocks, GL_STATIC_DRAW);

	data_in.sendToGPU(0);
	data_out.sendToGPU(1);
	sums.sendToGPU(2);
	_computePrefixSum.bind();
	_computePrefixSum.setInt("n", values.size());
	_computePrefixSum.unbind();
	_computePrefixSum.dispatch(numBlocks, 1, 1);

	
	sums.sendToGPU(0);
	incr.sendToGPU(1);
	_computePrefixSumAux.bind();
	_computePrefixSumAux.setInt("n", numBlocks);
	_computePrefixSumAux.unbind();
	_computePrefixSumAux.dispatch(1, 1, 1);

	numThreads = 1024;
	numBlocks = (numThreads + values.size() - 1) / numThreads;
	incr.sendToGPU(0);
	data_out.sendToGPU(1);
	_computeUniformIncrement.bind();
	_computeUniformIncrement.setInt("n", values.size());
	_computeUniformIncrement.unbind();
	_computeUniformIncrement.dispatch(numBlocks, 1, 1);


	std::vector<unsigned int> prefixSum = data_out.retrieveBuffer();
	std::vector<unsigned int> sums_host = sums.retrieveBuffer();
	std::vector<unsigned int> incr_host = incr.retrieveBuffer();

	for (int i = 0; i < values.size(); ++i)
	{
		if (values_out[i] != prefixSum[i]) std::cout << "wwhy???" << std::endl;
	}
	std::cout << "here" << std::endl;
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