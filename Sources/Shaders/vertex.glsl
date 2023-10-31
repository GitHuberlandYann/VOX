#version 150 core

/* specifications is packed
 * 0xF = x coord
 * 0xF0 = y coord
 * 0x100 = x break
 * 0x200 = xoffset
 * 0x400 = yoffset
 * 0xF000 y break
 * 0xFF0000 shading [0:100]
 * 0xF00000 x pos offset
 * 0xf000000 y pos offset
*/
in int specifications;
/* posistion.z is packed
 * 0xFF actual z pos
 * 0x300 corner light
 * 0xF0000 block light
 * 0xF00000 sky light
*/
in ivec3 position;

uniform mat4 view;
uniform mat4 proj;
uniform int internal_light;

out vec2 Texcoord;
out vec2 Breakcoord;
out float FaceShadow;
out float zDist;

void main()
{
	// gl_Position = proj * view * vec4(position, 1.0);
	gl_Position = proj * view * vec4(position.x + ((specifications >> 24) & 0xF) / 16.0f, position.y + ((specifications >> 28) & 0xF) / 16.0f, position.z & 0xFF, 1.0);
	Texcoord = vec2(((specifications & 0xF) - ((specifications >> 9) & 0x1) * 0.01f) / 16.0f, (((specifications >> 4) & 0xF) - ((specifications >> 10) & 0x1) * 0.01f) / 16.0f);
	Breakcoord = vec2((15.0f + ((specifications >> 8) & 0x1) * 0.99f) / 16.0f, (((specifications >> 12) & 0xF) - ((specifications >> 10) & 0x1) * 0.01f) / 16.0f);
	int blockLight = ((position.z >> 24) & 0xF);
	int skyLight = internal_light - (15 - ((position.z >> 28) & 0xF));
	int shadow = 15 - max(blockLight, skyLight);
	int cornerLight = ((position.z >> 16) & 3);
	FaceShadow = max(0.05, (max(10, ((specifications >> 16) & 0xFF) - 7 * shadow) - 17 * cornerLight) / 100.0f);
	zDist = gl_Position.z;
}
