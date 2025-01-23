#include "ComputeShader.h"

ComputeShader::ComputeShader(const std::string& shader)
	: Shader(shader, GL_COMPUTE_SHADER) {}

void  ComputeShader::dispatch(GLuint x, GLuint y, GLuint z)
{
	glUseProgram(id_);
	glDispatchCompute(x, y, z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}