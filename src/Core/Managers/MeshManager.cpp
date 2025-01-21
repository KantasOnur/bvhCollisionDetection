#include "MeshManager.h"

MeshManager& MeshManager::getInstance()
{
	static MeshManager meshManager;
	return meshManager;
}

void MeshManager::_addMesh(const std::string& obj)
{
	MeshLoader::Mesh m = MeshLoader::Load(obj);
	m_objToMesh[obj] = std::make_unique<MeshLoader::Mesh>(m);
}

const MeshLoader::Mesh* MeshManager::retrieveMesh(const std::string& obj)
{
	if (m_objToMesh.find(obj) == m_objToMesh.end()) _addMesh(obj);
	return m_objToMesh[obj].get();
}