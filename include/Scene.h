#pragma once
#include <string>
#include "Entity.h"
#include <vector>
#include "Camera.h"
#include "CollisionHandler.h"

class Scene
{
private:
	std::vector<std::unique_ptr<Entity>> m_entities;
	CollisionHandler m_collisionHandler;
	Camera m_camera;
public:
	Scene(const Window& window);
	~Scene() = default;
	void render();
	void addSubject(std::unique_ptr<Entity>&& mesh);
};