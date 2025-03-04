#include "BVH.h"
#include "EntityManager.h"
#include <iostream>
#include <algorithm>
#include <bit>
#include <random>
#include "Gui.h"

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