#version 150 core

in int specifications; // 0xF = x coord, 0xF0 = y coord, 0x100 = x break, 0x200 = xoffset, 0x400 = yoffset, 0xF000 y break, 0xFF0000 shading [0:100], 0xF00000 x pos offset, 0xf000000 y pos offset
in ivec3 position;

uniform mat4 view;
uniform mat4 proj;

out vec2 Texcoord;
out vec2 Breakcoord;
out float FaceShadow;
out float zDist;

void main()
{
	// gl_Position = proj * view * vec4(position, 1.0);
	gl_Position = proj * view * vec4(position.x + ((specifications >> 24) & 0xF) / 16.0f, position.y + ((specifications >> 28) & 0xF) / 16.0f, position.z, 1.0);
	Texcoord = vec2(((specifications & 0xF) - ((specifications >> 9) & 0x1) * 0.01f) / 16.0f, (((specifications >> 4) & 0xF) - ((specifications >> 10) & 0x1) * 0.01f) / 16.0f);
	Breakcoord = vec2((15.0f + ((specifications >> 8) & 0x1) * 0.99f) / 16.0f, (((specifications >> 12) & 0xF) - ((specifications >> 10) & 0x1) * 0.01f) / 16.0f);
	FaceShadow = ((specifications >> 16) & 0xFF) / 100.0f;
	zDist = gl_Position.z;
	// 0xF = x coord, 0xF0 = y coord, 0x100 = x break, 0x200 = xoffset, 0x400 = yoffset, 0xF000 = y break, 0xFF0000 = shading [0:100], 0xF00000 x pos offset, 0xf000000 y pos offset
}
