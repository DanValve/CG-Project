#shader vertex
#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoords;

uniform mat4 CLOUDSMVP;
out vec2 atexcoords;
void main() {

	gl_Position = CLOUDSMVP *vec4(position, 1.0f);
	atexcoords = texcoords;
};

#shader fragment
#version 430 core
//----------------------------------------------------------------------------------------

layout(location = 0) out vec4 color;
uniform float offset;
in vec2 atexcoords;
uniform sampler2D cloudtex;

void main() {
	vec2 updatedTexCoords = vec2(0.0f, offset) + atexcoords;
	vec4 texColor = texture(cloudtex, updatedTexCoords);
	color = texColor;


};

