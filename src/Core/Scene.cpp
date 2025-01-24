#include "Scene.h"
#include "Entity.h"
#include <iostream>
#include "Gui.h"

Scene::Scene(const Window& window)
	: m_camera(window.getAspectRatio()), m_collisionHandler(&m_entities)
{
	addSubject(std::make_unique<Entity>("armadillo"));
	addSubject(std::make_unique<Entity>("armadillo", glm::vec3{ 1.0f, 1.0f, 0.0f }));
}

void Scene::addSubject(std::unique_ptr<Entity>&& mesh)
{
	m_entities.emplace_back(std::move(mesh));
}

void Scene::render()
{
	m_collisionHandler.checkCollisions();

	ImGui::Begin("Entity Parameters");
	m_entities[0]->draw(m_camera);
	m_entities[1]->draw(m_camera);
	ImGui::End();
}