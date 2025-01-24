#pragma once
#include "Entity.h"
#include <memory>
#include "ComputeShader.h"

class CollisionHandler
{
private:
	std::vector<std::unique_ptr<Entity>>* m_sceneEntities;
	ComputeShader naive = ComputeShader("Naive");

private:
	bool _isAABBCollided(const int& i, const int& j);
	void _handleCollision(const int& i, const int& j);
public:
	CollisionHandler(std::vector<std::unique_ptr<Entity>>* sceneEntities);
	void checkCollisions();
};