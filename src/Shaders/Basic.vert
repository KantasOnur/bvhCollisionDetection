#version 430 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec4 vertexColor;


uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec4 fragColor;
out vec4 fragNormal;
out vec4 fragPosition;

void main()
{
    fragColor = vertexColor;
    fragPosition = modelMatrix * vertexPosition;
    fragNormal = vertexNormal;

   	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;
}