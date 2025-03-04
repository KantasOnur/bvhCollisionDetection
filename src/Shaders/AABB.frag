#version 430 core

in vec3 fragColour;
out vec4 color;
void main()
{
	color = vec4(fragColour, 1.0f);
}