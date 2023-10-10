#version 150 core

in ivec4 position;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 color;

out vec3 Color;

void main()
{
	gl_Position = proj * view * vec4(position.xy, position.z - (position.w / 8.0f), 1.0);
	Color = color;
}
