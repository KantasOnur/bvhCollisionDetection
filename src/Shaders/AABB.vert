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

uniform vec4 min;
uniform vec4 max;

void main()
{
    vec4 localPosition = vertices[gl_VertexID].position;
    // Map from [0, 1] to AABB space
    vec4 worldPosition = min + localPosition * (max - min);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * worldPosition;
}