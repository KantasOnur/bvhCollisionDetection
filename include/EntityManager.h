#pragma once
#include "Entity.h"
#include <memory>
class EntityManager
{
private:
	std::unordered_map<unsigned int, Entity*> m_idToEntity;
	EntityManager() = default;

public:
	static EntityManager& getInstance()
	{
		static EntityManager manager;
		return manager;
	}
	~EntityManager();	

	void addEntity(const unsigned int& id, Entity* entity);

	void removeEntity(const unsigned int& id);

	Entity& getEntity(const unsigned int& id) const;
};