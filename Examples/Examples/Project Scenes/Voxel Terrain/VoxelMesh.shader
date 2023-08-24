#shader vertex
#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in float blockID;
layout(location = 2) in vec4 showLaterals;
layout(location = 3) in vec2 showTopBottom;
//Interfaces
out VERTEX_DATA{
int blockID;
vec4 showLaterals;
vec2 showTopBottom;
}out_data;

struct BlockData {
	int textureID;
	bool opaque;
	bool multitexture;
	float offset;
	float dummy;

};

uniform BlockData blocks[16];
//Uniforms
uniform mat4 MVP;


void main() {

	gl_Position = vec4(position, 1.0f);
	out_data.blockID = int(blockID);
	out_data.showLaterals = showLaterals;
	out_data.showTopBottom = showTopBottom;
};

#shader fragment
#version 430 core
//----------------------------------------------------------------------------------------

layout(location = 0) out vec4 color;

//Interfaces
in GEOMETRY_DATA{
vec4 color;//delete later!!!!!!!!!!!!
vec2 texCoords;//to get the right coordinates
flat int textureID;//to access the right texture
}in_data;


struct BlockData {
	int textureID;
	bool opaque;
	bool multitexture;
	float offset;
	float dummy;

};

uniform BlockData blocks[16];
uniform int TotalTexInSpriteSheet[3];//one dimensional stride
uniform sampler2D u_texture[3];

void main() {
	int index = in_data.textureID;
	vec4 texColor = texture(u_texture[index], in_data.texCoords);
	color = texColor;
	

};


#shader geometry
#version 430 core
//----------------------------------------------------------------------------------------

layout(points) in;
layout(triangle_strip, max_vertices = 28) out;

//Interfaces
in VERTEX_DATA{
int blockID;
vec4 showLaterals;
vec2 showTopBottom;
}in_data[];

out GEOMETRY_DATA{
vec4 color;//delete later!!!!!!!!!!!!
vec2 texCoords;//to get the right coordinates
flat int textureID;//to access the right texture, check if need to change to int
}out_data;

//Struct
struct BlockData {
	int textureID;
	bool opaque;
	bool multitexture;
	float offset;//one dimensional offset, might change to float
	float dummy;

};

uniform BlockData blocks[16];
uniform int TotalTexInSpriteSheet[3];//one dimensional stride
uniform mat4 MVP;

//Variables
vec4 initPos;
vec4 vertexPos;
float texOffset;
float step;
//Constants
const int FRONT = 0;
const int BACK = 1;
const int LEFT = 2;
const int RIGHT = 3;
const int TOP = 4;
const int BOTTOM = 5;
//Functions
void generateSide(const int side);

void main() {


	int blockID;
	int textureID;
	bool multitexture;
	int totalTextures;

	//array mode
	blockID = in_data[0].blockID;
	textureID = blocks[blockID].textureID;
	multitexture = blocks[blockID].multitexture;
	totalTextures = TotalTexInSpriteSheet[textureID];
	texOffset = blocks[blockID].offset / totalTextures;//xoffset might need to cast totaltextures to a float
	step = 1.0f / totalTextures;
	out_data.textureID = textureID;


	if (in_data[0].showTopBottom.x == 1.0f) {
		generateSide(TOP);
	}


	if (multitexture) {
		texOffset += step;
	}
	if (in_data[0].showTopBottom.y == 1.0f) {
		generateSide(BOTTOM);
	}

	if (multitexture) {
		texOffset += step;
	}
	//Laterals
	if (in_data[0].showLaterals.x == 1.0f) {
		generateSide(FRONT);
	}
	if (in_data[0].showLaterals.y == 1.0f) {
		generateSide(BACK);
	}
	if (in_data[0].showLaterals.z == 1.0f) {
		generateSide(LEFT);
	}
	if (in_data[0].showLaterals.w == 1.0f) {
		generateSide(RIGHT);
	}


	

};


void generateSide(const int side) {

	switch (side) {
	case FRONT:
		initPos = vec4(-0.5f, 0.5f, 0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 0.0f);
		EmitVertex();

		vertexPos = initPos + vec4(1.0f, 0.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(1.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 0.0f);
		EmitVertex();

		EndPrimitive();

		break;
	case BACK:
		initPos = vec4(0.5f, 0.5f, -0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 0.0f);
		EmitVertex();

		vertexPos = initPos + vec4(-1.0f, 0.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(-1.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 0.0f);
		EmitVertex();

		EndPrimitive();
		break;
	case LEFT:
		initPos = vec4(-0.5f, 0.5f, -0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 0.0f);
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 0.0f);
		EmitVertex();

		EndPrimitive();
		break;
	case RIGHT:
		initPos = vec4(0.5f, 0.5f, 0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 0.0f);
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, 0.0f, -1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, -1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 0.0f);
		EmitVertex();

		EndPrimitive();
		break;
	case TOP:
		initPos = vec4(-0.5f, 0.5f, -0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 1.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 1.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 0.0f);
		EmitVertex();

		vertexPos = initPos + vec4(1.0f, 0.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 1.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(1.0f, 0.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(0.0f, 1.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 0.0f);
		EmitVertex();

		EndPrimitive();
		break;
	case BOTTOM:
		initPos = vec4(0.5f, -0.5f, -0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset, 0.0f);
		EmitVertex();

		vertexPos = initPos + vec4(-1.0f, 0.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 1.0f);
		EmitVertex();

		vertexPos = initPos + vec4(-1.0f, 0.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = MVP * vertexPos;
		out_data.color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
		out_data.texCoords = vec2(texOffset + step, 0.0f);
		EmitVertex();

		EndPrimitive();
		break;
	}
}
