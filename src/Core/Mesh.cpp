#include "Mesh.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "MeshManager.h"
Mesh::Mesh(const std::string& obj)
    : m_mesh(MeshManager::getInstance().retrieveMesh(obj)), 
    m_verticiesSSBO(GL_SHADER_STORAGE_BUFFER, m_mesh->verticies, GL_STATIC_DRAW),
    m_indiciesSSBO(GL_SHADER_STORAGE_BUFFER, m_mesh->indicies, GL_STATIC_DRAW)
{
    m_mesh = MeshManager::getInstance().retrieveMesh(obj);
    initBuffers();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);
}

void Mesh::initBuffers()
{ 
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_verticiesSSBO.getID());

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MeshLoader::Vertex), (void*)offsetof(MeshLoader::Vertex, position));    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(MeshLoader::Vertex), (void*)offsetof(MeshLoader::Vertex, normal));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(MeshLoader::Vertex), (void*)offsetof(MeshLoader::Vertex, color));

    glEnableVertexArrayAttrib(m_vao, 0);
    glEnableVertexArrayAttrib(m_vao, 1);
    glEnableVertexArrayAttrib(m_vao, 2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indiciesSSBO.getID());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::_drawMesh(const Camera& camera)
{
    glBindVertexArray(m_vao);
    m_verticiesSSBO.bind();
    m_verticiesSSBO.sendToGPU(0);
    glDrawElements(GL_TRIANGLES, (int)m_mesh->indicies.size(), GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindVertexArray(0);
}