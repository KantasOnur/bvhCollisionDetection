#include "CollisionHandler.h"
#include <iostream>


/*
	It doesnt work for intersection between a 3d mesh and a 2d mesh
*/

CollisionHandler::CollisionHandler(std::vector<std::unique_ptr<Entity>>* sceneEntities)
	: m_sceneEntities(sceneEntities) {}

void CollisionHandler::checkCollisions()
{
	/*
	for (int i = 0; i < m_sceneEntities->size(); ++i)
	{
		for (int j = 0; j < m_sceneEntities->size(); ++j)
		{
			if (i == j) continue;
			if (_isAABBCollided(i, j)) _handleCollision(i, j);
		}
	}
	*/
	_handleCollision(0, 1);
}

bool CollisionHandler::_isAABBCollided(const int& i, const int& j)
{
	const Entity* entity_i = (*m_sceneEntities)[i].get();
	const Entity* entity_j = (*m_sceneEntities)[j].get();

	glm::vec3 min_i, max_i;
	glm::vec3 min_j, max_j;

	entity_i->getMinMax(min_i, max_i);
	entity_j->getMinMax(min_j, max_j);

	return (
		min_i.x <= max_j.x &&
		max_i.x >= min_j.x &&
		min_i.y <= max_j.y &&
		max_i.y >= min_j.y &&
		min_i.z <= max_j.z &&
		max_i.z >= min_j.z
		);
}

void CollisionHandler::_handleCollision(const int& i, const int& j)
{
	
	const Entity* entity_i = (*m_sceneEntities)[i].get();
	const Entity* entity_j = (*m_sceneEntities)[j].get();

	const auto& verticies_i = entity_i->getVerticies();
	const auto& verticies_j = entity_j->getVerticies();

	const auto& indicies_i = entity_i->getIndicies();
	const auto& indicies_j = entity_j->getIndicies();
	

	verticies_i.sendToGPU(0);
	indicies_i.sendToGPU(1);

	verticies_j.sendToGPU(2);
	indicies_j.sendToGPU(3);

	naive.bind();
	naive.setUInt("nVertex_i", verticies_i.getSize());
	naive.setUInt("nVertex_j", verticies_j.getSize());

	naive.setUInt("nIndex_i", indicies_i.getSize());
	naive.setUInt("nIndex_j", indicies_j.getSize());

	naive.setMatrix4f("modelMatrix_i", entity_i->getModelMatrix());
	naive.setMatrix4f("modelMatrix_j", entity_j->getModelMatrix());
	naive.unbind();

	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + verticies_i.getSize() - 1) / threadsPerBlock;
	naive.dispatch(numBlocks, 1, 1);
}