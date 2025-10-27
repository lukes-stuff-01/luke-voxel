#version 330 core

layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoordIn;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int size;
uniform sampler2D textures;
uniform float threshold;
uniform bvec3 flipSelect;
uniform int orientation;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;
vec3 unwrap();
vec2 generate2dCoords(int order, vec3 coordinatesIn);
vec3 coords3dFlip(bvec3 flip, vec3 coordinatesIn);

void main()
{
	vec3 coords = unwrap();
	vec2 coords2d = generate2dCoords(orientation, coords);
	vec4 colour = texture(textures, coords2d);
	float average = (colour.x + colour.y + colour.z)/3;
	if(average > threshold)
	{
		gl_Position = vec4(2.0f, 2.0f, 2.0f, 0.0f);//This sets the vertex to an invalid position, meaning it can't render and will therefore be clipped. Also shouldn't generate any fragments as they are all points(maybe?)
	}
	else
	{
	coords = coords3dFlip(flipSelect, coords);
	fragPos = vec3(model * vec4(positions, 1.0)) + coords;
	normal = normals;
	gl_Position = projection * view * (vec4(fragPos, 1.0f));
	}
};

vec3 unwrap(){
	float coordinate_total = gl_InstanceID;
	float z = floor(gl_InstanceID / (pow(size, 2)));
	coordinate_total -= z * pow(size, 2);
	float y = floor(coordinate_total / size);
	float x = mod(coordinate_total, size);
	return vec3(x,y,z);
};

vec2 generate2dCoords(int order, vec3 coordinatesIn){
	vec3 coordOutput = vec3(coordinatesIn);
	switch (order) {
		case 0:
		coordOutput = coordOutput.xyz;
		break;
		case 1:
		coordOutput = coordOutput.xzy;
		break;
		case 2:
		coordOutput = coordOutput.yxz;
		break;
		case 3:
		coordOutput = coordOutput.yzx;
		break;
		case 4:
		coordOutput = coordOutput.zxy;
		break;
		case 5:
		coordOutput = coordOutput.zyx;
		break;
	};
	return vec2((coordOutput.x+(coordOutput.z*size)) / pow(size, 2), (coordOutput.y / size));
};
vec3 coords3dFlip(bvec3 flip, vec3 coordinatesIn) {
	vec3 coordOutput = coordinatesIn;
	if (flip.x){
		coordOutput = vec3(size - coordOutput.x, coordOutput.y, coordOutput.z);
	};
	if (flip.y){
		coordOutput = vec3(coordOutput.x, size - coordOutput.y, coordOutput.z);
	};
	if (flip.z){
		coordOutput = vec3(coordOutput.x, coordOutput.y, size - coordOutput.z);
	};
	return coordOutput;
};