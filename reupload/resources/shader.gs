#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;//I have too many slurs to say to that max_vertices thing fuck you bro istg my shit did not compile FOR NO REASON!!!!!!!!!

in vec3[3] normal;
in vec3[3] fragPos;
in vec2[3] texCoord;


out vec3 normalFrag;
out vec3 fragPosFrag;
out vec2 texCoordFrag;

void main() {
	for(int i = 0; i < 3; i++)
	{
		gl_Position = gl_in[i].gl_Position;
		normalFrag = normal[i];
		fragPosFrag = fragPos[i];
		texCoordFrag = texCoord[i];
		EmitVertex();
	}
	EndPrimitive();
}