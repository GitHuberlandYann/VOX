#version 150 core

in ivec3 position;

uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * vec4(position, 1.0);
}
