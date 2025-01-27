#include "Entity.h"
#include "Gui.h"
#include <format>

Entity::Entity(const std::string& obj, const glm::vec3& position)
	: Mesh(obj), m_position(position), m_aabb(m_mesh, &m_modelMatrix) 
{
	m_id = m_instance++;
	_updateModelMatrix();
}

void Entity::draw(const Camera& camera)
{
	_drawGui();
		
	//m_aabb.draw(camera);
	m_shader.bind();

	m_shader.setMatrix4f("projectionMatrix", camera.getProjection());
	m_shader.setMatrix4f("viewMatrix", camera.getView());
	m_shader.setMatrix4f("modelMatrix", m_modelMatrix);

	_drawMesh(camera);
	m_shader.unbind();
}

void Entity::getMinMax(glm::vec3& min, glm::vec3& max) const
{
	min = m_modelMatrix * m_mesh->min;
	max = m_modelMatrix * m_mesh->max;
}

const GLBuffer<MeshLoader::Vertex>& Entity::getVerticies() const
{
	return m_verticiesSSBO;
}

const GLBuffer<unsigned int>& Entity::getIndicies() const
{
	return m_indiciesSSBO;
}

void Entity::_updateModelMatrix()
{
	m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
	m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(m_scale));
}


void Entity::_drawGui()
{
	if (ImGui::SliderFloat3(std::format("Entity {}: Position", m_id).c_str(), &m_position[0], -5.0f, 5.0f)) _updateModelMatrix();
	if (ImGui::SliderFloat(std::format("Entity {}: Scale", m_id).c_str(), &m_scale, 1.0f, 4.0f)) _updateModelMatrix();
}