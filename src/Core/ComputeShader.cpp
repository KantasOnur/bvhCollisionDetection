#include "ComputeShader.h"
#include <iostream>

ComputeShader::ComputeShader(const std::string& shader)
	: Shader(std::string(SRC) + "/Shaders/Compute/" + shader + ".comp", GL_COMPUTE_SHADER) {}

void  ComputeShader::dispatch(GLuint x, GLuint y, GLuint z)
{
	//std::cout << "dispatched" << std::endl;
	bind();
	glDispatchCompute(x, y, z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	
	//glFinish();
	unbind();
	//std::cout << "complted" << std::endl;
}