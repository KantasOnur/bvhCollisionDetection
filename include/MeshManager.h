#pragma once
#include <unordered_map>
#include <string>
#include "MeshLoader.h"
#include <memory>

class MeshManager
{
private:
	// Give the obj file name, get the verticies and indicies.
	std::unordered_map<std::string, std::unique_ptr<MeshLoader::Mesh>> m_objToMesh;
private:
	MeshManager() = default;
	void _addMesh(const std::string& obj);
public:
	static MeshManager& getInstance();
	
	/* If the obj is loaded once then it is retrieved, otherwise obj is loaded here. */
	const MeshLoader::Mesh* retrieveMesh(const std::string& obj);
};