#include "Entity.h"
#include "Gui.h"
#include <format>
#include <iostream>

Entity::Entity(const std::string& obj, const glm::vec3& position)
	: Mesh(obj), m_position(position), m_aabb(m_mesh, &m_modelMatrix) 
{
	m_id = m_instance++;
	_updateModelMatrix();
}

void Entity::draw(const Camera& camera)
{
	
	_drawGui();
	


	if (m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_shader.bind();

	m_shader.setMatrix4f("projectionMatrix", camera.getProjection());
	m_shader.setMatrix4f("viewMatrix", camera.getView());
	m_shader.setMatrix4f("modelMatrix", m_modelMatrix);
	m_shader.setVec3f("cameraPosition", camera.getPosition());
	_drawMesh(camera);
	m_shader.unbind();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
	if (ImGui::InputFloat3(std::format("Entity {}: Position", m_id).c_str(), &m_position[0])) _updateModelMatrix();
	if (ImGui::InputFloat3(std::format("Entity {}: Scale", m_id).c_str(), &m_scale)) _updateModelMatrix();

	ImGui::Text("Triangles: %d, Vertices: %d", m_indiciesSSBO.getSize() / 3, m_verticiesSSBO.getSize());

	if (ImGui::Checkbox(std::format("Wireframe {}", m_id).c_str(), &m_wireframe));
}