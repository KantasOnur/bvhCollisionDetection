#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>
#include "Shader.h"
#include <vector>

class AABB : public Mesh
{
private:
	glm::vec4 m_min, m_max;
	glm::vec3 summed_min, summed_max;
	float m_scaleFactor = 1.0f;
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);

	const float& m_scale;
	const glm::vec3& m_position;

	Shader m_shader = Shader("AABB");

private:
	void _updateModelMatrix();

public:
	AABB(const MeshLoader::Mesh* mesh, const float& scale, const glm::vec3& position);
	~AABB() = default;
	void draw(const Camera& camera) override;
	void MinkowskiSum(const AABB& aabb);
	void getMinMax(glm::vec3& min, glm::vec3& max) const;
	bool rayMinkowksiSumIntersection(const AABB& aabb, const glm::vec3& rayDirection,
		const glm::vec3& rayPosition, float& t);

	static bool rayIntersectsAABB(const glm::vec3& origin, const glm::vec3& direction,
		const glm::vec3& boxMin, const glm::vec3& boxMax, float& tMinOut);
	glm::vec3 getMidPoint();
};