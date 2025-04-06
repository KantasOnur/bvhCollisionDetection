#include "ConstantSpeedSimulator.h"
#include "EntityManager.h"
#include "Gui.h"
#include <string>
#include <format>
#include <iostream>

ConstantSpeedSimulator::ConstantSpeedSimulator()
	: Mesh("vector") {}

void ConstantSpeedSimulator::subEntity(const unsigned int& id)
{
	//m_idToVelocity[id] = { 0.0f, 0.0f, 0.0f };
	m_entityToVelocity[id] = { 0.0f, 0.0f, 0.0f };
}

void ConstantSpeedSimulator::draw(const Camera& camera)
{
	_drawGui();
	/*
	for (auto& it : m_entityToVelocity)
	{

		Entity& entity = EntityManager::getInstance().getEntity(it.first);

		glm::vec3 position = entity.getMidPoint();
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);

		m_shader.bind();

		m_shader.setMatrix4f("projectionMatrix", camera.getProjection());
		m_shader.setMatrix4f("viewMatrix", camera.getView());
		m_shader.setMatrix4f("modelMatrix", modelMatrix);
		m_shader.setVec3f("cameraPosition", camera.getPosition());
		
		m_shader.setVec3f("dx", it.second * m_dt);
		_drawMesh(camera);
		m_shader.unbind();
	}
	*/
}

void ConstantSpeedSimulator::step(const std::vector<unsigned int>& sceneEntities)
{
	/*
	static bool autoStep = false;
	
	ImGui::Begin("Simulator Parameters");
	ImGui::Checkbox("Auto Step", &autoStep);
	
	if ( autoStep || ImGui::Button("Step", { 100.0f, 100.0f }))
	{
		
		for (auto& it : m_entityToVelocity)
		{
			Entity& entity = EntityManager::getInstance().getEntity(it.first);
			entity.constructBVH();
			glm::vec3 velocity = it.second;
			glm::vec3 position = entity.getPosition();
			position += velocity * m_dt;

			m_collisionHandler.checkCollisions(entity.getID(), sceneEntities, position);

			// there is a bug here checking then setting the position causes collision viewer to be one frame late.
			//if(!m_collisionHandler.checkCollisions(entity.getID(), sceneEntities)) entity.setPosition(position); 
		}
	}
	
	ImGui::End();
	*/

	for (auto& it : m_entityToVelocity)
	{
		Entity& entity = EntityManager::getInstance().getEntity(it.first);
		glm::vec3 position = entity.getPosition();
		if (ImGui::SliderFloat3(std::format("Entity {}: Position", it.first).c_str(), &position[0], 0.0f, 10.0f))
			entity.setPosition(position);
		m_collisionHandler.checkCollisions(entity.getID(), sceneEntities, position);
	}
}

void ConstantSpeedSimulator::_drawGui()
{
	ImGui::Begin("Simulator Parameters");
	ImGui::InputFloat("Time step", &m_dt);
	for (auto& it : m_entityToVelocity)
	{
		/*
		glm::vec3& v = it.second;
		if (ImGui::SliderFloat3(std::format("Entity {} velocity", it.first).c_str(), &v[0], -1.0f, 1.0f))
		{
			std::cout << "here" << std::endl;
		}
		*/
		Entity& entity = EntityManager::getInstance().getEntity(it.first);
		glm::vec3 position = entity.getPosition();
		if (ImGui::SliderFloat3(std::format("Entity {}: Position", it.first).c_str(), &position[0], 0.0f, 10.0f))
		{
			entity.setPosition(position);
		}

	}
	ImGui::End();
}


glm::vec3 ConstantSpeedSimulator::getVelocity(const unsigned int& id)
{
	return m_entityToVelocity.at(id);
}