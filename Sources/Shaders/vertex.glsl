#version 150 core

/* specifications is packed
 * 0xF = x coord in texture atlas
 * 0xF0 = y coord in texture atlas
 * 0xF00 = y offset in texture atlas
 * 0xF000 = y coord of breaking frame
 * 0x1 << 16 = x offset of breaking frame
 * 0x1 << 17 x texture half-pixel correction
 * 0x1 << 18 y texture half-pixel correction
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
uniform float min_brightness;

out vec2 Texcoord;
out vec2 Breakcoord;
out float FaceShadow;
out float zDist;

void main()
{
	gl_Position = proj * view * vec4(position, 1.0);
	float x_half = (((specifications & (1 << 17)) == 0) ? 0.0001220703125 : -0.0001220703125);
	float y_half = (((specifications & (1 << 18)) == 0) ? 0.0001220703125 : -0.0001220703125);
	Texcoord = vec2((specifications & 0xF) / 16.0f + x_half, ((specifications & 0xF0) + ((specifications >> 8) & 0xF)) / 256.0f + y_half);
	Breakcoord = vec2((15 + ((specifications >> 16) & 0x1)) / 16.0f + x_half, ((specifications >> 12) & 0xF) / 16.0f + y_half);
	int blockLight = ((specifications >> 24) & 0xF);
	int skyLight = internal_light - (15 - ((specifications >> 28) & 0xF));
	int shadow = 15 - max(blockLight, skyLight);
	int cornerLight = ((specifications >> 22) & 3);
	int faceLight = 100 - ((((specifications >> 19) & 0x7) > 0) ? 8 + (((specifications >> 19) & 0x7) << 2) : 0);
	// if (((specifications >> 19) & 0x7) > 0) { // TODO get rid of this if statement
	// 	faceLight -= 8 + (((specifications >> 19) & 0x7) << 2);
	// }
	FaceShadow = max(min_brightness, (max(10, faceLight - 7 * shadow) - 17 * cornerLight) * 0.01f);
	zDist = gl_Position.z;
}
