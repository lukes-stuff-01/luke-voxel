#version 330 core

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 FragColor;


void main()
{
	FragColor = vec4(abs(normal), 1.0);
};