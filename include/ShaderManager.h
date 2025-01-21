#pragma once
#include <unordered_map>
#include <string>
#include "OpenGL.h"

class ShaderManager
{
private:
	std::unordered_map<std::string, unsigned int> m_nameToProgram;
private:
	ShaderManager() = default;
public:
	static ShaderManager& getInstance();
	void addProgram(const std::string& name, const GLenum& type, const unsigned int& program);
	void removeProgram(const std::string& name, const GLenum& type, const unsigned int& program);
	unsigned int getProgram(const std::string& name, const GLenum& type) const;
};