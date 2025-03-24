#include "Shader.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

Shader::Shader(const std::string& shader)
    : shaderName(shader)
{

    const std::string vertexShaderPath = std::string(SRC) + "/Shaders/" + shader + ".vert";
    const std::string fragmentShaderPath = std::string(SRC) + "/Shaders/" + shader + ".frag";

    unsigned int vShader = createShader(GL_VERTEX_SHADER, vertexShaderPath);
    unsigned int fShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderPath);
    createProgram({ vShader, fShader });
}

Shader::Shader(const std::string& shaderPath, const GLenum& type)
{
    shaderName = shaderPath;
    unsigned int shaderID = createShader(type, shaderPath);
    createProgram({ shaderID });
}

Shader::~Shader()
{
    glDeleteProgram(id_);
}

static std::string processShaderFile(const std::string& sourcePath)
{
    std::ifstream stream(sourcePath);
    std::string source;

    if (stream)
    {
        std::stringstream shaderStream;
        shaderStream << stream.rdbuf();
        stream.close();
        source = shaderStream.str();
    }
    else
    {
        std::cout << "Failed to read shader at: " << sourcePath << std::endl;
        exit(EXIT_FAILURE);
    }
    std::regex includeRegex("(#include <([a-zA-Z]+)>\n)");
    for (std::smatch sm; std::regex_search(source, sm, includeRegex);)
    {
        std::string includeFileSourcePath = std::string(SRC) + "/Shaders/include/" + sm[2].str() + ".glsl";
        std::string includeFileSource = processShaderFile(includeFileSourcePath);
        source.replace(sm.position(0), sm.length(0), includeFileSource);
    }
    return source;
}

unsigned int Shader::createShader(GLenum type, const std::string& sourcePath)
{
    std::string source = processShaderFile(sourcePath);


    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cerr << "Error compiling shader at path: " << sourcePath << " due to: \n" << log << std::endl;
        return -1;
    }

    return shader;
}

void Shader::createProgram(const std::vector<unsigned int>& shaders)
{
    unsigned int program = glCreateProgram();
    for (unsigned int shader : shaders)
        glAttachShader(program, shader);

    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(program, 512, NULL, log);
        std::cout << this << std::endl;
        std::cerr << "Error compiling program: " << log << std::endl;
        return;
    }

    // Shaders can be discarded after linking
    for (unsigned int shader : shaders)
        glDeleteShader(shader);

    id_ = program;
    unbind();
}

void Shader::bind() const
{
    glUseProgram(id_);
}
void Shader::unbind() const
{
    glUseProgram(0);
}

GLint Shader::getUniformLocation(const std::string& name)
{
    if (uniformToLocation_.find(name) == uniformToLocation_.end())
    {
        uniformToLocation_[name] = glGetUniformLocation(id_, name.c_str());
    }
    return uniformToLocation_[name];
}

void Shader::setMatrix4f(const std::string& name, const glm::mat4& val)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &val[0][0]);
}

void Shader::setVec3f(const std::string& name, const glm::vec3& val)
{
    glUniform3fv(getUniformLocation(name), 1, &val[0]);
}

void Shader::setVec4f(const std::string& name, const glm::vec4& val)
{
    glUniform4fv(getUniformLocation(name), 1, &val[0]);
}

void Shader::setFloat1f(const std::string& name, const float& val)
{
    glUniform1f(getUniformLocation(name), val);
}

void Shader::setInt(const std::string& name, const int& val)
{
    glUniform1i(getUniformLocation(name), val);
}

void Shader::setUInt(const std::string& name, const unsigned int& val)
{
    glUniform1ui(getUniformLocation(name), val);
}