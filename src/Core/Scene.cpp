#include "Scene.h"
#include "Entity.h"
#include <iostream>
#include "Gui.h"

Scene::Scene(const Window& window)
	: m_camera(window.getAspectRatio()), m_collisionHandler(&m_entities)
{
	addSubject(std::make_unique<Entity>("armadillo"));
	addSubject(std::make_unique<Entity>("dragon", glm::vec3{ 1.0f, 1.0f, 0.0f }));
}

void Scene::addSubject(std::unique_ptr<Entity>&& mesh)
{
	m_entities.emplace_back(std::move(mesh));
}

void Scene::render()
{
	//static bool once = true;
	//if(once) m_collisionHandler.checkCollisions();
	//once = false;

	static bool suzanneWireFrame = false;

	m_collisionHandler.checkCollisions();

	ImGui::Begin("Entity Parameters");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_entities[0]->draw(m_camera);

	if (ImGui::Checkbox("WireFrame", &suzanneWireFrame)) suzanneWireFrame != suzanneWireFrame;
	if(suzanneWireFrame) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_entities[1]->draw(m_camera);
	ImGui::End();

}