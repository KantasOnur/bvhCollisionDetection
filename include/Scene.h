#pragma once
#include <string>
#include "Entity.h"
#include <vector>
#include "Camera.h"

class Scene
{
private:
	std::vector<std::unique_ptr<Entity>> m_entities;
	Camera m_camera;
public:
	Scene(const Window& window);
	void render();
	void addSubject(std::unique_ptr<Entity>&& mesh);
};