#include "EntityManager.h"

EntityManager::~EntityManager()
{
	for (auto& it : m_idToEntity)
	{
		Entity* entity = m_idToEntity.at(it.first);
		delete entity;
		entity = nullptr;
	}
}

void EntityManager::addEntity(const unsigned int& id, Entity* entity)
{
	m_idToEntity[id] = entity;
}

void EntityManager::removeEntity(const unsigned int& id)
{
	Entity* entity = m_idToEntity.at(id);
	delete entity;
	entity = nullptr;
	m_idToEntity.erase(id);
}

Entity& EntityManager::getEntity(const unsigned int& id) const
{
	return *m_idToEntity.at(id);
}

