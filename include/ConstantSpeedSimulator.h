#pragma once
#include "Entity.h"
#include "Shader.h"
#include "CollisionHandler.h"

class ConstantSpeedSimulator : public Mesh
{
private:
	static inline float m_dt = 0.1f;
	static inline std::unordered_map<unsigned int, glm::vec3> m_entityToVelocity;
	bool proceedStep = false;

	glm::vec3 vector = { 0.0f, 1.0f, 0.0f };

	Shader m_shader = Shader("Vector");
	CollisionHandler m_collisionHandler;
private:
	void _drawGui();
public:
	ConstantSpeedSimulator();
	~ConstantSpeedSimulator() = default;

	void subEntity(const unsigned int& id);
	void draw(const Camera& camera) override;
	
	void step(const std::vector<unsigned int>& sceneEntities);
	static glm::vec3 getVelocity(const unsigned int& id);
	static float getStepTime() { return m_dt; };
};