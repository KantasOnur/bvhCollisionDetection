#include "Scene.h"
#include "Entity.h"
#include <iostream>
#include "Gui.h"

bool isIntersect(const glm::vec3& min_i, const glm::vec3& max_i,
	const glm::vec3& min_j, const glm::vec3& max_j)
{
	return (
		min_i.x <= max_j.x &&
		max_i.x >= min_j.x &&
		min_i.y <= max_j.y &&
		max_i.y >= min_j.y &&
		min_i.z <= max_j.z &&
		max_i.z >= min_j.z
		);
}



Scene::Scene(const Window& window)
	: m_camera(window.getAspectRatio())
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

	m_entities[0]->draw(m_camera);
	m_entities[1]->draw(m_camera);
	
	ImGui::Begin("Entity Parameters");
	glm::vec3 min_i, max_i, min_j, max_j;
	for (int i = 0; i < m_entities.size(); ++i)
	{
		m_entities[i]->getMinMax(min_i, max_i);
		for (int j = 0; j < m_entities.size(); ++j)
		{
			if (i == j) continue;
			m_entities[j]->getMinMax(min_j, max_j);
			
			if (isIntersect(min_i, max_i, min_j, max_j))
			{
				std::cout << "intersect" << std::endl;
			}
		}
	}
	ImGui::End();
}