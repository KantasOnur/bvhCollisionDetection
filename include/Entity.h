#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>
#include <string>
#include "Shader.h"
#include "AABB.h"
#include "BVH.h"

class Entity : public Mesh
{
private:

	bool m_wireframe = false;

	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::vec3 m_position;
	glm::vec3 m_pivot;

	float m_scale = 1.0f;
	bool m_drawBVH = false;
	AABB m_aabb;
	std::unique_ptr<BVH> m_bvh;

	Shader m_shader = Shader("Basic");

	inline static int m_instance = 0;
	int m_id;
private:
	void _updateModelMatrix();
	void _drawGui(const Camera& camera);

public:
	Entity(const std::string& obj, const glm::vec3& position = { 0.0f, 0.0f, 0.0f });
	~Entity() = default;

	void draw(const Camera& camera) override;
	void getMinMaxWorld(glm::vec3& min, glm::vec3& max) const;
	void getMinMaxModel(glm::vec3& min, glm::vec3& max) const;
	const GLBuffer<MeshLoader::Vertex>& getVerticies() const;
	const GLBuffer<unsigned int>& getIndicies() const;
	glm::mat4 getModelMatrix() const { return m_modelMatrix; };
	void setPosition(const glm::vec3& position) { m_position = position; _updateModelMatrix(); };
	glm::vec3 getPosition() const { return m_position; };
	float getScale() const { return m_scale; }
	unsigned int getID() const { return m_id; };
	AABB& getAABB() { return m_aabb; }
	glm::vec3 getMidPoint() { return m_aabb.getMidPoint(); };
	void constructBVH() { m_bvh->constructBVH(); };
	void bindBVH(const unsigned int& i0, const unsigned int& i1) { m_bvh->bindToLocation(i0, i1); };
	void getBVHSize(unsigned int& numLeaves, unsigned int& numInternals);
};