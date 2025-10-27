#version 330 core

in vec3 normalFrag;
in vec3 fragPosFrag;
in vec2 texCoordFrag;

out vec4 FragColor;


void main()
{
	FragColor = vec4(abs(normalFrag), 1.0);
};