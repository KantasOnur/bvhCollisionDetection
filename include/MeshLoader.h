#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>



namespace MeshLoader
{
	struct Vertex
	{
		glm::vec4 position = { 0.0f, 0.0f, 0.0f, 0.0f };
		glm::vec4 normal = { 0.0f, 0.0f, 0.0f, 0.0f };
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct Mesh
	{
		std::vector<Vertex> verticies;
		std::vector<unsigned int> indicies;
		glm::vec4 min;
		glm::vec4 max;
	};

	MeshLoader::Mesh Load(const std::string& obj);
}
