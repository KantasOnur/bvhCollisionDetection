#version 430 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec4 vertexColor;


uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 dx;
 
out vec4 fragColor;
out vec4 fragNormal;
out vec4 fragPosition;

vec4 getQuaternion()
{
    vec3 u = vec3(0.0f, 1.0f, 0.0f); // arrow mesh points upwards.

    vec3 a = cross(u, dx);
    vec4 q;
    q.xyz = a;
    q.w = dot(u, dx) + sqrt(pow(length(u), 2) * pow(length(dx), 2));
    return normalize(q);
}

vec3 rotateVectorByQuaternion(vec3 v, vec4 q) {
    // Extract the vector (xyz) and scalar (w) parts of the quaternion
    vec3 u = q.xyz;
    float s = q.w;
    
    // Compute the rotated vector
    return 2.0 * dot(u, v) * u
         + (s * s - dot(u, u)) * v
         + 2.0 * s * cross(u, v);
}


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
    
    vec4 quaternion = getQuaternion();

    
    fragColor = vertexColor;
    //fragPosition = modelMatrix * vertexPosition;
    fragNormal = vertexNormal;
    fragPosition = scale(vec3(1.0f, length(dx), 1.0f)) * vertexPosition;
    fragPosition = vec4(rotateVectorByQuaternion(fragPosition.xyz, quaternion), 1.0f);
    fragPosition = modelMatrix * fragPosition;
   	gl_Position = projectionMatrix * viewMatrix * fragPosition;
}