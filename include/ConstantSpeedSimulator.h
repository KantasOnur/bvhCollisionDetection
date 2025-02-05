#pragma once
#include "Entity.h"
#include "Shader.h"

class ConstantSpeedSimulator : public Mesh
{
private:
	float m_dt = 0.1f;
	//std::unordered_map<unsigned int, glm::vec3> m_idToVelocity;
	std::unordered_map<Entity*, glm::vec3> m_entityToVelocity;
	bool proceedStep = false;

	glm::vec3 vector = { 0.0f, 1.0f, 0.0f };

	Shader m_shader = Shader("Vector");
private:
	void _drawGui();
public:
	ConstantSpeedSimulator();
	~ConstantSpeedSimulator() = default;

	void subEntity(Entity* entity);
	void draw(const Camera& camera) override;
	
	void step(const std::vector<std::unique_ptr<Entity>>& sceneEntities);
};