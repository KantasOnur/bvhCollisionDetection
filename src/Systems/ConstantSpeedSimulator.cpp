#include "ConstantSpeedSimulator.h"
#include "Gui.h"
#include <string>
#include <format>
#include <iostream>

ConstantSpeedSimulator::ConstantSpeedSimulator()
	: Mesh("vector") {}

void ConstantSpeedSimulator::subEntity(Entity* entity)
{
	//m_idToVelocity[id] = { 0.0f, 0.0f, 0.0f };
	m_entityToVelocity[entity] = { 0.0f, 0.0f, 0.0f };
}

void ConstantSpeedSimulator::draw(const Camera& camera)
{
	_drawGui();

	for (auto& it : m_entityToVelocity)
	{
		Entity* entity = it.first;
		glm::vec3 position = entity->getPosition();
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
		//glm::vec3 velocity = it.second;
		//glm::vec3 cross = glm::cross({ 1.0f, 0.0f, 0.0f }, velocity);
		//if(id == 0) std::cout << cross.x << " " << cross.y << " " << cross.z << std::endl;

		m_shader.bind();

		m_shader.setMatrix4f("projectionMatrix", camera.getProjection());
		m_shader.setMatrix4f("viewMatrix", camera.getView());
		m_shader.setMatrix4f("modelMatrix", modelMatrix);
		m_shader.setVec3f("cameraPosition", camera.getPosition());
		
		m_shader.setVec3f("dx", it.second * m_dt);
		_drawMesh(camera);
		m_shader.unbind();
	}
}

void ConstantSpeedSimulator::step(const std::vector<std::unique_ptr<Entity>>& sceneEntities)
{
	static bool autoStep = false;

	ImGui::Begin("Simulator Parameters");
	ImGui::Checkbox("Auto Step", &autoStep);
	if ( autoStep || ImGui::Button("Step", { 100.0f, 100.0f }))
	{
		for (auto& it : m_entityToVelocity)
		{
			Entity* entity = it.first;
			glm::vec3 velocity = it.second;
			glm::vec3 position = entity->getPosition();
			position += velocity * m_dt;
			entity->setPosition(position);
		}
	}
	ImGui::End();
}

void ConstantSpeedSimulator::_drawGui()
{
	ImGui::Begin("Simulator Parameters");
	ImGui::InputFloat("Time step", &m_dt);
	for (auto& it : m_entityToVelocity)
	{
		glm::vec3& v = it.second;
		ImGui::SliderFloat3(std::format("Entity {} velocity", it.first->getID()).c_str(), &v[0], -1.0f, 1.0f);
	}
	ImGui::End();
}
