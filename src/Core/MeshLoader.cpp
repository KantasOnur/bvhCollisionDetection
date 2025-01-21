#include "MeshLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <regex>

MeshLoader::Vertex constructVertex(const std::cmatch& m,
	const std::vector<glm::vec4>& v,
	const std::vector<glm::vec4>& vn)
{
	MeshLoader::Vertex vertex;
	
	try
	{
		vertex.position = v[std::stoi(m[2]) - 1];
	}
	catch (const std::invalid_argument& ia){}
	try
	{
		vertex.normal = vn[std::stoi(m[6]) - 1];
	}
	catch (const std::invalid_argument& ia) {}
	return vertex;
}

void updateMinMax(glm::vec4& min, glm::vec4& max, const glm::vec4& vertexPosition)
{
	if (min.w == 0.0f)
	{
		min = vertexPosition;
		max = vertexPosition;
		return;
	}

	min.x = glm::min(min.x, vertexPosition.x);
	min.y = glm::min(min.y, vertexPosition.y);
	min.z = glm::min(min.z, vertexPosition.z);

	max.x = glm::max(max.x, vertexPosition.x);
	max.y = glm::max(max.y, vertexPosition.y);
	max.z = glm::max(max.z, vertexPosition.z);
}

MeshLoader::Mesh MeshLoader::Load(const std::string& obj)
{
	std::string objPath = std::string(SRC) + "/../assets/" + obj + ".obj";
	Mesh mesh;
	std::vector<glm::vec4> v, vn;

	std::unordered_map<std::string, unsigned int> objVertexToIndex;
	
	std::ifstream objFile(objPath);

	std::string line;
	std::stringstream ss;

	float x, y, z;
	unsigned int index = 0;

	std::regex objVertexPattern(R"(^(\s*)(\d*)(\/)(\d*)(\/)(\d*)$)");
	std::cmatch m;

	std::vector<Vertex> verticies;
	std::vector<unsigned int> indicies;

	glm::vec4 min = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 max = { 0.0f, 0.0f, 0.0f, 0.0f };

	while (getline(objFile, line))
	{
		ss = std::stringstream(line);
		std::string temp;
		ss >> temp;
		if (temp == "v")
		{
			ss >> x;
			ss >> y;
			ss >> z;
			glm::vec4 vertex = { x, y, z, 1.0f };
			v.push_back(vertex);
			
			updateMinMax(min, max, vertex);
		}
		else if (temp == "vn")
		{
			ss >> x;
			ss >> y;
			ss >> z;
			vn.push_back({ x, y, z, 1.0f });
		}
		else if (temp == "f")
		{
			for (int i = 0; i < 3; ++i)
			{
				ss >> temp;
				if (objVertexToIndex.find(temp) == objVertexToIndex.end())
				{
					objVertexToIndex[temp] = index++;
					if (regex_match(temp.c_str(), m, objVertexPattern))
					{
						//std::cout << "here" << std::endl;
						Vertex vertex = constructVertex(m, v, vn);
						verticies.push_back(vertex);
					}
					else
					{
						std::cout << "Error: " << obj << " is faulty." << std::endl;
						return { std::vector<Vertex>(), std::vector<unsigned int>() };
					}
				}
				indicies.push_back(objVertexToIndex[temp]);
			}
		}
	}

	
	return { verticies, indicies, min, max };
}