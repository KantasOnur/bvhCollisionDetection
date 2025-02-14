#include "Scene.h"
#include "Entity.h"
#include <iostream>
#include "Gui.h"

Scene::Scene(const Window& window)
	: m_camera(window.getAspectRatio())
{
	addSubject(std::make_unique<Entity>("dragon"));
	addSubject(std::make_unique<Entity>("dragon", glm::vec3{ 0.0f, 1.0f, 0.0f }));
	m_simulator.subEntity(m_entities[0].get());
	m_simulator.subEntity(m_entities[1].get());
}

void Scene::addSubject(std::unique_ptr<Entity>&& mesh)
{
	m_entities.emplace_back(std::move(mesh));
}

void Scene::render()
{
	m_simulator.step(m_entities);
	m_simulator.draw(m_camera);
	m_collisionHandler.checkCollisions(m_entities);
	for (auto& entitiy : m_entities)
	{
		entitiy->draw(m_camera);
	}

	//m_collisionHandler.checkCollisions(m_entities);
	/*
	m_entities[0]->draw(m_camera);
	m_entities[1]->draw(m_camera);
	*/
}