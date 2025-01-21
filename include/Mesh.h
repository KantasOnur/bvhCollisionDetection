#pragma once
#include "Shader.h"
#include <glm/glm.hpp>
#include "Camera.h"
#include <memory>
#include <vector>
#include "MeshLoader.h"

class Mesh
{
protected:

    const MeshLoader::Mesh* m_mesh;

    unsigned int m_vao, m_ibo;
    unsigned int m_verticiesSSBO, m_indiciesSSBO;

protected:
    void _drawMesh(const Camera& camera);
private:
    void initBuffers();
public:
    Mesh(const std::string& obj);
    ~Mesh();
    virtual void draw(const Camera& camera) = 0;
};