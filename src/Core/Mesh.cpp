#include "Mesh.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "MeshManager.h"
Mesh::Mesh(const std::string& obj)
{
    m_mesh = MeshManager::getInstance().retrieveMesh(obj);
    initBuffers();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_verticiesSSBO);
    glDeleteBuffers(1, &m_ibo);
}

void Mesh::initBuffers()
{ 

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_verticiesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_verticiesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_mesh->verticies.size() * sizeof(MeshLoader::Vertex), m_mesh->verticies.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_verticiesSSBO);

    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh->indicies.size() * sizeof(unsigned int), m_mesh->indicies.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::_drawMesh(const Camera& camera)
{
    glBindVertexArray(m_vao);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_verticiesSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_verticiesSSBO);
    glDrawElements(GL_TRIANGLES, (int)m_mesh->indicies.size(), GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindVertexArray(0);
}
/*
void Mesh::draw(const Camera& camera)
{
    
    shader_.bind();
    shader_.setMatrix4f("projectionMatrix", camera.getProjection());
    shader_.setMatrix4f("viewMatrix", camera.getView());
    
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, (int)indices_.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    shader_.unbind();
}
*/
/*
void Mesh::_constructAABB()
{
    if (vertices_.size() == 0) return;
    glm::vec3 min, max = vertices_[0].position;
    for (const auto& vertex : vertices_)
    {
        min.x = std::min(min.x, vertex.position.x);
        min.y = std::min(min.y, vertex.position.y);
        min.z = std::min(min.z, vertex.position.z);

        max.x = std::max(max.x, vertex.position.x);
        max.y = std::max(max.y, vertex.position.y);
        max.z = std::max(max.z, vertex.position.z);
    }
}
*/