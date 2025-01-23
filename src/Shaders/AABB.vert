#version 430 core

/*
layout(std430, binding = 0) buffer VertexBuffer 
{
    struct Vertex {
        vec4 position;
		vec4 normal;
    } vertices[];
};
*/

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec4 vertexColor;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec4 min;
uniform vec4 max;

void main()
{
    // Map from [0, 1] to AABB space
    vec4 worldPosition = min + vertexPosition * (max - min);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * worldPosition;
}