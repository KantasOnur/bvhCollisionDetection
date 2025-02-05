#pragma once
#include "Entity.h"
#include <memory>
#include "ComputeShader.h"

class CollisionHandler
{
private:
	//std::vector<std::unique_ptr<Entity>>* m_sceneEntities;
	ComputeShader naive = ComputeShader("Naive");

private:
	bool _isAABBCollided(const std::vector<std::unique_ptr<Entity>>& sceneEntities, const int& i, const int& j);
	void _handleCollision(const std::vector<std::unique_ptr<Entity>>& sceneEntities, const int& i, const int& j);
public:
	CollisionHandler() = default;
	void checkCollisions(const std::vector<std::unique_ptr<Entity>>& sceneEntities);
};