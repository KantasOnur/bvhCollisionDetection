#include "BVH.h"
#include "EntityManager.h"
#include <iostream>
#include <algorithm>
#include <bit>
#include <random>
#include "Gui.h"
#include <format>
#include <chrono>

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

BVH::BVH(const unsigned int& entityID)
	: m_entityID(entityID), Mesh("cube")
{
	const unsigned int triangleCount = EntityManager::getInstance().getTriangleCount(entityID);
	m_leafNodes = std::make_unique<GLBuffer<LeafNode>>
		(GL_SHADER_STORAGE_BUFFER, nullptr, triangleCount, GL_STATIC_DRAW);

	m_internalNodes = std::make_unique<GLBuffer<InternalNode>>
		(GL_SHADER_STORAGE_BUFFER, std::vector<InternalNode>(m_leafNodes->getSize() - 1), GL_STATIC_DRAW);

	m_sorter = std::make_unique<RadixSort>(m_leafNodes->getSize());
	m_numLeaves = m_leafNodes->getSize();
	m_numInternals = m_internalNodes->getSize();
	/*
	constructBVH();
	std::vector<InternalNode> nodes = m_internalNodes->retrieveBuffer();
	std::vector<LeafNode> lf = m_leafNodes->retrieveBuffer();
	*/
}
BVH::~BVH()
{

}

void BVH::draw(const Camera& camera)
{
	//constructBVH();
	std::vector<InternalNode> nodes = m_internalNodes->retrieveBuffer();
	auto a = m_leafNodes->retrieveBuffer();
	ImGui::InputInt(std::format("BVH depth {}", m_entityID).c_str(), &m_depth, 1);
	drawRecursive(camera, m_depth, nodes, 0);
}

void BVH::constructBVH()
{
	_constructLeafNodes();
	_constructInternalNodes();
	_constructBounds();
}

void BVH::drawRecursive(const Camera& camera, const unsigned int depth, 
	const std::vector<InternalNode>& nodes, const unsigned int currentNode)
{
	if (depth == 0)
	{
		glm::mat4 modelMatrix = EntityManager::getInstance().getEntity(m_entityID).getModelMatrix();
		shader.bind();
		shader.setMatrix4f("projectionMatrix", camera.getProjection());
		shader.setMatrix4f("viewMatrix", camera.getView());
		shader.setMatrix4f("modelMatrix", modelMatrix);

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


void BVH::_constructLeafNodes()
{
	_getMortonCodes();
	//std::vector<LeafNode> lf = m_leafNodes->retrieveBuffer();
	m_sorter->sort(*m_leafNodes);
	//lf = m_leafNodes->retrieveBuffer();

}

void BVH::_constructInternalNodes()
{

	m_internalNodes->sendToGPU(0);
	m_leafNodes->sendToGPU(1);

	_computeInternalsNodes.bind();
	_computeInternalsNodes.setUInt("numLeaves", m_leafNodes->getSize());
	_computeInternalsNodes.setUInt("numInternals", m_leafNodes->getSize() - 1);
	_computeInternalsNodes.unbind();

	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + m_leafNodes->getSize() - 1) / threadsPerBlock;
	_computeInternalsNodes.dispatch(numBlocks, 1, 1);
}

void BVH::_constructBounds()
{
	m_internalNodes->sendToGPU(0);
	m_leafNodes->sendToGPU(1);

	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + m_leafNodes->getSize() - 1) / threadsPerBlock;

	_computeBounds.bind();
	_computeBounds.setUInt("numLeafs", m_leafNodes->getSize());
	_computeBounds.unbind();

	_computeBounds.dispatch(numBlocks, 1, 1);	
}

void BVH::_getMortonCodes()
{
	glm::vec3 min, max;
	Entity& entity = EntityManager::getInstance().getEntity(m_entityID);
	const GLBuffer<MeshLoader::Vertex>& verticies = entity.getVerticies();
	const GLBuffer<unsigned int>& indicies = entity.getIndicies();
	entity.getMinMaxModel(min, max);
	glm::mat4 modelMatrix = entity.getModelMatrix();

	const unsigned int triangleCount = indicies.getSize() / 3;
	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + triangleCount - 1) / threadsPerBlock;

	m_leafNodes->sendToGPU(0);
	verticies.sendToGPU(1);
	indicies.sendToGPU(2);

	_computeMortonCodes.bind();
	_computeMortonCodes.setVec3f("u_min", min);
	_computeMortonCodes.setVec3f("u_max", max);
	_computeMortonCodes.setUInt("triangleCount", triangleCount);
	_computeMortonCodes.setMatrix4f("u_modelMatrix", modelMatrix);
	_computeMortonCodes.unbind();

	_computeMortonCodes.dispatch(numBlocks, 1, 1);
}

void BVH::bindToLocation(const unsigned int& i0, const unsigned& i1)
{
	m_leafNodes->sendToGPU(i0);
	m_internalNodes->sendToGPU(i1);
}