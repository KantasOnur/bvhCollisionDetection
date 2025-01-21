#version 430 core

layout(std430, binding = 0) buffer VertexBuffer 
{
    struct Vertex {
        vec4 position;
		vec4 normal;
    } vertices[];
};

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
   	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertices[gl_VertexID].position;
}