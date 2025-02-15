#pragma once
#include <string>
#include "Entity.h"
#include <vector>
#include "Camera.h"
#include "CollisionHandler.h"
#include "ConstantSpeedSimulator.h"

class Scene
{
private:
	std::vector<unsigned int> m_entities;
	ConstantSpeedSimulator m_simulator;

	Camera m_camera;
public:
	Scene(const Window& window);
	~Scene() = default;
	void render();
	void addSubject(const unsigned int& id);
};