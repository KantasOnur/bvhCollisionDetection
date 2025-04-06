#include "CollisionHandler.h"
#include <iostream>
#include "ConstantSpeedSimulator.h"
#include "EntityManager.h"

bool CollisionHandler::_checkSweepCollision(const unsigned int& i_id, const unsigned int& j_id)
{
	Entity& entity_i = EntityManager::getInstance().getEntity(i_id);
	Entity& entity_j = EntityManager::getInstance().getEntity(j_id);

	
	AABB& aabb_i = entity_i.getAABB();
	AABB& aabb_j = entity_j.getAABB();

	glm::vec3 vel_i = ConstantSpeedSimulator::getVelocity(i_id);
	glm::vec3 vel_j = ConstantSpeedSimulator::getVelocity(j_id);
	glm::vec3 dx = vel_i * ConstantSpeedSimulator::getStepTime();
	
	glm::vec3 position = entity_i.getPosition();
	glm::vec3 rayOrigin = aabb_i.getMidPoint();
	glm::vec3 rayEnd = rayOrigin + dx;

	float t;
	bool collided = aabb_j.rayMinkowksiSumIntersection(aabb_i, rayEnd, rayOrigin, t);
	if (collided)
	{
		glm::vec3 targetMidPoint = rayOrigin + dx * t;
		glm::vec3 pivotToMid = position - rayOrigin;
		glm::vec3 newPosition = targetMidPoint + pivotToMid;
		entity_i.setPosition(newPosition);
	}
	return collided && t > 0.0001f;
}

bool CollisionHandler::checkCollisions(const unsigned int& id, 
	const std::vector<unsigned int>& sceneEntities, const glm::vec3& newPosition)
{
	for (int j = 0; j < sceneEntities.size(); ++j)
	{
		if (id == sceneEntities[j]) continue;
		const unsigned int j_id = sceneEntities[j];
		//bool missedCollision = _checkSweepCollision(id, j_id);
		if (_isAABBCollided(id, j_id))
		{
			// Position is updated here, because doing so in the simulator causes the _handleCollision function to use the old position
			//EntityManager::getInstance().getEntity(id).setPosition(newPosition);
			
			//_handleCollisionNaive(id, j_id);
			_handleCollisionBVH(id, j_id);
			return true;
		}
	}
	return false;
}

bool CollisionHandler::_isAABBCollided(const unsigned int& i_id, const unsigned int& j_id)
{
	Entity& entity_i = EntityManager::getInstance().getEntity(i_id);
	Entity& entity_j = EntityManager::getInstance().getEntity(j_id);

	glm::vec3 min_i, max_i;
	glm::vec3 min_j, max_j;

	entity_i.getMinMaxWorld(min_i, max_i);
	entity_j.getMinMaxWorld(min_j, max_j);

	return (
		min_i.x <= max_j.x &&
		max_i.x >= min_j.x &&
		min_i.y <= max_j.y &&
		max_i.y >= min_j.y &&
		min_i.z <= max_j.z &&
		max_i.z >= min_j.z
		);
}

void CollisionHandler::_handleCollisionNaive(const unsigned int& i_id, const unsigned int& j_id)

{
	Entity& entity_i = EntityManager::getInstance().getEntity(i_id);
	Entity& entity_j = EntityManager::getInstance().getEntity(j_id);


	const auto& verticies_i = entity_i.getVerticies();
	const auto& verticies_j = entity_j.getVerticies();

	const auto& indicies_i = entity_i.getIndicies();
	const auto& indicies_j = entity_j.getIndicies();
	

	verticies_i.sendToGPU(0);
	indicies_i.sendToGPU(1);

	verticies_j.sendToGPU(2);
	indicies_j.sendToGPU(3);

	naive.bind();
	naive.setUInt("nVertex_i", verticies_i.getSize());
	naive.setUInt("nVertex_j", verticies_j.getSize());

	naive.setUInt("nIndex_i", indicies_i.getSize());
	naive.setUInt("nIndex_j", indicies_j.getSize());

	naive.setMatrix4f("modelMatrix_i", entity_i.getModelMatrix());
	naive.setMatrix4f("modelMatrix_j", entity_j.getModelMatrix());
	naive.unbind();

	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + verticies_i.getSize() - 1) / threadsPerBlock;
	naive.dispatch(numBlocks, 1, 1);
	
}

void CollisionHandler::_handleCollisionBVH(const unsigned int& i_id, const unsigned int& j_id)
{

	unsigned int numLeaves, numInternals;

	Entity& entity_i = EntityManager::getInstance().getEntity(i_id);
	Entity& entity_j = EntityManager::getInstance().getEntity(j_id);

	entity_j.constructBVH();
	entity_j.getBVHSize(numLeaves, numInternals);


	const auto& verticies_i = entity_i.getVerticies();
	const auto& verticies_j = entity_j.getVerticies();

	const auto& indicies_i = entity_i.getIndicies();
	const auto& indicies_j = entity_j.getIndicies();

	verticies_i.sendToGPU(0);
	indicies_i.sendToGPU(1);

	verticies_j.sendToGPU(2);
	indicies_j.sendToGPU(3);
	entity_j.bindBVH(4, 5);

	bvh.bind();
	bvh.setUInt("nVertex_i", verticies_i.getSize());
	bvh.setUInt("nVertex_j", verticies_j.getSize());

	bvh.setUInt("nIndex_i", indicies_i.getSize());
	bvh.setUInt("nIndex_j", indicies_j.getSize());
	bvh.setUInt("nLeaves", numLeaves);
	bvh.setUInt("nInternals", numInternals);
	bvh.setMatrix4f("modelMatrix_i", entity_i.getModelMatrix());
	bvh.setMatrix4f("modelMatrix_j", entity_j.getModelMatrix());
	bvh.unbind();

	// shoot rays from each vertex of entity_i to entity_j to see if the associated vertex
	// is contained within j.

	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + verticies_i.getSize() - 1) / threadsPerBlock;
	bvh.dispatch(numBlocks, 1, 1);
	/*
	const auto& verticies_i = entity_i.getVerticies();
	const auto& verticies_j = entity_j.getVerticies();

	const auto& indicies_i = entity_i.getIndicies();
	const auto& indicies_j = entity_j.getIndicies();

	verticies_i.sendToGPU(0);
	indicies_i.sendToGPU(1);

	verticies_j.sendToGPU(2);
	indicies_j.sendToGPU(3);
	entity_j.bindBVH(4, 5);

	naive.bind();
	naive.setUInt("nVertex_i", verticies_i.getSize());
	naive.setUInt("nVertex_j", verticies_j.getSize());

	naive.setUInt("nIndex_i", indicies_i.getSize());
	naive.setUInt("nIndex_j", indicies_j.getSize());

	naive.setMatrix4f("modelMatrix_i", entity_i.getModelMatrix());
	naive.setMatrix4f("modelMatrix_j", entity_j.getModelMatrix());
	naive.unbind();

	const unsigned int threadsPerBlock = 1024;
	const size_t numBlocks = (threadsPerBlock + verticies_i.getSize() - 1) / threadsPerBlock;
	naive.dispatch(numBlocks, 1, 1);
	*/
}