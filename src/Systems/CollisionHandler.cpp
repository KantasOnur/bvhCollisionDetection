#include "CollisionHandler.h"
#include <iostream>


CollisionHandler::CollisionHandler(std::vector<std::unique_ptr<Entity>>* sceneEntities)
	: m_sceneEntities(sceneEntities) {}

void CollisionHandler::checkCollisions()
{
	static int a = 0;
	for (int i = 0; i < m_sceneEntities->size(); ++i)
	{
		for (int j = 0; j < m_sceneEntities->size(); ++j)
		{
			if (i == j) continue;
			if (_isAABBCollided(i, j)) std::cout << "collided" << a++ << std::endl;
		}
	}
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