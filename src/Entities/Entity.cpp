#include "Entity.h"

Entity::Entity(const std::string& obj, const glm::vec3& position)
	: Mesh(obj), m_position(position), m_aabb(m_mesh, &m_modelMatrix) 
{
	_updateModelMatrix();
}

/*
void Entity::_checkCollisions()
{

	for (int i = 0; i < m_mesh->indicies.size(); i += 3)
	{

	}
}
*/

void Entity::draw(const Camera& camera)
{
	m_aabb.draw(camera);
	m_shader.bind();

	m_shader.setMatrix4f("projectionMatrix", camera.getProjection());
	m_shader.setMatrix4f("viewMatrix", camera.getView());
	m_shader.setMatrix4f("modelMatrix", m_modelMatrix);

	_drawMesh(camera);
	m_shader.unbind();
}

void Entity::getMinMax(glm::vec3& min, glm::vec3& max)
{
	min = m_modelMatrix * m_mesh->min;
	max = m_modelMatrix * m_mesh->max;
}

void Entity::_updateModelMatrix()
{
	m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
}
