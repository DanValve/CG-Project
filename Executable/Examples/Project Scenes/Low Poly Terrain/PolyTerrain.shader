#shader vertex
#version 430 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
uniform mat4 MVP;
out vec3 v_Color;
void main()
{

	gl_Position = MVP * vec4(a_Position, 1.0f);
	v_Color = a_Color;
};

#shader fragment
#version 430 core

layout(location = 0) out vec4 color;

in vec3 v_Color;

void main()
{

	color = vec4(v_Color,1.0f);

};


