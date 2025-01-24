#include "ComputeShader.h"

ComputeShader::ComputeShader(const std::string& shader)
	: Shader(std::string(SRC) + "/Shaders/Compute/" + shader + ".comp", GL_COMPUTE_SHADER) {}

void  ComputeShader::dispatch(GLuint x, GLuint y, GLuint z)
{
	glUseProgram(id_);
	glDispatchCompute(x, y, z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}