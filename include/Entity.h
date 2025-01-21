#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>
#include <string>
#include "Shader.h"
#include "AABB.h"

class Entity : public Mesh
{
private:
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::vec3 m_position;

	AABB m_aabb;
	Shader m_shader = Shader("Basic");

private:
	void _updateModelMatrix();
	//void _checkCollisions();
public:
	Entity(const std::string& obj, const glm::vec3& position = {0.0f, 0.0f, 0.0f});
	~Entity() = default;
	void draw(const Camera& camera) override;
	void getMinMax(glm::vec3& min, glm::vec3& max);
};