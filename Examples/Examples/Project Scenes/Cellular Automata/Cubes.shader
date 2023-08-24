#shader vertex
#version 430 core
layout(location = 0) in vec3 position;

void main() {

	gl_Position = vec4(position, 1.0f);

};

#shader fragment
#version 430 core
//----------------------------------------------------------------------------------------
layout(location = 0) out vec4 Color;

in GEOMETRY_DATA{
flat int Index;
flat vec3 Color;
}in_data;
uniform int Stride;
uniform int TotalStates;
layout(binding = 2, std430) buffer Buffer2 { int Output[]; };

void main() {
	
	int outState = Output[in_data.Index];
	//Color = vec4(colors[outState],1.0f);
	//Color = vec4(in_data.Color,1.0f);
	float cVal = float(outState) / (TotalStates-1);
	Color = vec4(cVal,cVal,cVal,1.0f);
	//Color = vec4(1.0f, 1.0f, 0.0f, 1.0f);


};


#shader geometry
#version 430 core
//----------------------------------------------------------------------------------------

layout(points) in;
layout(triangle_strip, max_vertices = 28) out;

out GEOMETRY_DATA{
flat int Index;
flat vec3 Color;
}out_data;

uniform mat4 u_MVP;
uniform int Stride;
layout(binding = 2, std430) buffer Buffer2 { int Output[]; };
//Variables
vec4 initPos;
vec4 vertexPos;

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

	int id = int(gl_in[0].gl_Position.x + gl_in[0].gl_Position.y * Stride + gl_in[0].gl_Position.z * Stride * Stride);
	out_data.Index = id;
	int outState = Output[id];
	out_data.Color = vec3(gl_in[0].gl_Position.x / Stride, gl_in[0].gl_Position.y /Stride, gl_in[0].gl_Position.z /Stride);
	if (outState != 0) {
		generateSide(TOP);
		generateSide(BOTTOM);
		generateSide(FRONT);
		generateSide(BACK);
		generateSide(LEFT);
		generateSide(RIGHT);
	}



};


void generateSide(const int side) {

	switch (side) {
	case FRONT:
		initPos = vec4(-0.5f, 0.5f, 0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;
	
		EmitVertex();

		vertexPos = initPos + vec4(1.0f, 0.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(1.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		EndPrimitive();

		break;
	case BACK:
		initPos = vec4(0.5f, 0.5f, -0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(-1.0f, 0.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(-1.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		EndPrimitive();
		break;
	case LEFT:
		initPos = vec4(-0.5f, 0.5f, -0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;
		EmitVertex();

		vertexPos = initPos + vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		EndPrimitive();
		break;
	case RIGHT:
		initPos = vec4(0.5f, 0.5f, 0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(0.0f, 0.0f, -1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(0.0f, -1.0f, -1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		EndPrimitive();
		break;
	case TOP:
		initPos = vec4(-0.5f, 0.5f, -0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(1.0f, 0.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(1.0f, 0.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		EndPrimitive();
		break;
	case BOTTOM:
		initPos = vec4(0.5f, -0.5f, -0.5f, 0.0f);

		vertexPos = initPos;
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(-1.0f, 0.0f, 0.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		vertexPos = initPos + vec4(-1.0f, 0.0f, 1.0f, 0.0f);
		vertexPos += gl_in[0].gl_Position;
		gl_Position = u_MVP * vertexPos;

		EmitVertex();

		EndPrimitive();
		break;
	}
}
