#version 430 core

uniform vec3 cameraPosition;

in vec4 fragColor;
in vec4 fragNormal;
in vec4 fragPosition;

out vec4 color;
void main()
{
	vec3 lightColor = vec3(0.8f);

	float ambientStrength = 0.4f;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(fragNormal.xyz);
	vec3 lightDir = normalize(cameraPosition - fragPosition.xyz);

	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor;

	color = vec4((ambient + diffuse), 1.0f) * fragColor;
}