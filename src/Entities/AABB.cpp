#include "AABB.h"

AABB::AABB(const MeshLoader::Mesh* mesh, const glm::mat4* modelMatrix)
	: m_modelMatrix(modelMatrix), Mesh("cube")
{
	m_min = mesh->min;
	m_max = mesh->max;
}

void AABB::draw(const Camera& camera)
{
	m_shader.bind();
	m_shader.setMatrix4f("projectionMatrix", camera.getProjection());
	m_shader.setMatrix4f("viewMatrix", camera.getView());
	m_shader.setMatrix4f("modelMatrix", *m_modelMatrix);
	m_shader.setVec4f("min", m_min);
	m_shader.setVec4f("max", m_max);

	_drawMesh(camera);
	m_shader.unbind();
}