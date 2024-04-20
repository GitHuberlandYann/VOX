#version 150 core

in vec3 position;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 camPos;

out vec3 texCoord;

void main()
{
	gl_Position = proj * view * vec4(camPos + position, 1.0);
	gl_Position.z = gl_Position.w;
	texCoord = position;
}
