#version 150 core

/* specifications is packed
 * 0xF = x coord in texture atlas
 * 0xF0 = y coord in texture atlas
 * 0x100 = x coord of breaking frame
 * 0x200 = xoffset in texture atlas
 * 0x400 = yoffset in texture atlas
 * 0xF000 y coord break of breaking frame
 * 0x7 << 19 faceLight [0=100, 1=92, 2=88, 3=84, 4=80, 5=76]
 * 0x3 << 22 corner shade
 * 0xF000000 block light
 * 0xF0000000 sky light
*/
in int specifications;
in vec3 position;

uniform mat4 view;
uniform mat4 proj;
uniform int internal_light;

out vec2 Texcoord;
out vec2 Breakcoord;
out float FaceShadow;
out float zDist;

void main()
{
	gl_Position = proj * view * vec4(position, 1.0);
	Texcoord = vec2(((specifications & 0xF) - ((specifications >> 9) & 0x1) * 0.01f) / 16.0f, (((specifications >> 4) & 0xF) - ((specifications >> 10) & 0x1) * 0.01f) / 16.0f);
	Breakcoord = vec2((15.0f + ((specifications >> 8) & 0x1) * 0.99f) / 16.0f, (((specifications >> 12) & 0xF) - ((specifications >> 10) & 0x1) * 0.01f) / 16.0f);
	int blockLight = ((specifications >> 24) & 0xF);
	int skyLight = internal_light - (15 - ((specifications >> 28) & 0xF));
	int shadow = 15 - max(blockLight, skyLight);
	int cornerLight = ((specifications >> 22) & 3);
	int faceLight = 100;
	if (((specifications >> 19) & 0x7) > 0) {
		faceLight -= 8 + (((specifications >> 19) & 0x7) << 2);
	}
	FaceShadow = max(0.05, (max(10, faceLight - 7 * shadow) - 17 * cornerLight) / 100.0f);
	zDist = gl_Position.z;
}
