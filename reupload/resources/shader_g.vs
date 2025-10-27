#version 330 core

layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoordIn;
layout (location = 3) in vec3 offsets;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;

void main()
{
	fragPos = vec3(model * vec4(positions, 1.0)) + offsets;
	normal = normals;
	gl_Position = projection * view * (vec4(fragPos, 1.0f));
}