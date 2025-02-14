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
uniform float scaleFactor;

mat4 scale(vec3 s)
{
    return mat4(
        s.x, 0.0, 0.0, 0.0,
        0.0, s.y, 0.0, 0.0,
        0.0, 0.0, s.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}



void main()
{
    // Map from [0, 1] to AABB space
    vec4 worldPosition = min + vertexPosition * (max - min);
    
    //vec3 a = 2.0f
    //mat4 modelMatrix_ = modelMatrix * scale(vec3(scaleFactor));
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * worldPosition;
}