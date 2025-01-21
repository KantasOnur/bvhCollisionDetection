#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>
#include "Shader.h"
#include <vector>
class AABB : public Mesh
{
private:
	glm::vec4 m_min, m_max;
	const glm::mat4* m_modelMatrix;

	Shader m_shader = Shader("AABB");
public:
	AABB(const MeshLoader::Mesh* mesh, const glm::mat4* modelMatrix);
	~AABB() = default;
	void draw(const Camera& camera) override;
};