#include "Scene.h"
#include "Entity.h"
#include <iostream>
#include "Gui.h"
#include "EntityManager.h"

Scene::Scene(const Window& window)
	: m_camera(window.getAspectRatio())
{
	Entity* d0 = new Entity("dragon");
	//Entity* d1 = new Entity("armadillo", glm::vec3(0.0f, 1.0f, 0.0f));
	addSubject(d0->getID());
	//addSubject(d1->getID());
	m_simulator.subEntity(d0->getID());
	//m_simulator.subEntity(d1->getID());
}

void Scene::addSubject(const unsigned int& id)
{
	m_entities.push_back(id);
}

void Scene::render()
{
	m_simulator.step(m_entities);
	m_simulator.draw(m_camera);

	for (auto& id : m_entities)
	{
		EntityManager::getInstance().getEntity(id).draw(m_camera);
	}
}