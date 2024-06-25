#version 150 core

/** texture content: (21/32 bits used)
 * 0x00000FFF texture index in [0:4095]
 * 0x0000F000 x texture coord in [0:15]
 * 0x000F0000 y texture coord in [0:15]
 * 0x00100000 x texture half-pixel correction (1 << 20)
 * 0x00200000 y texture half-pixel correction (1 << 21)
 * 0xFFC00000 free
 */
in int texture;
/** light content: (14/32 bits used)
 * 0x0000000F block light
 * 0x000000F0 sky light
 * 0x00000700 faceLight [0=100, 1=92, 2=88, 3=84, 4=80, 5=76] (0x7 << 8)
 * 0x00003000 corner shader (0x3 << 12)
 * 0xFFFFC000 free
 */
in int light;
in vec3 position;

uniform mat4 view;
uniform mat4 proj;
uniform int internal_light;
uniform float min_brightness;

out vec3 TexCoords;
out float FaceShadow;
out float zDist;

const float one16th = 0.0625f;
const float half_pxl = 0.0001220703125f;

void main()
{
	gl_Position = proj * view * vec4(position, 1.0);

	float x_half = (((texture & (1 << 20)) == 0) ? half_pxl : one16th - half_pxl);
	float y_half = (((texture & (1 << 21)) == 0) ? half_pxl : one16th - half_pxl);
	TexCoords = vec3(((texture >> 12) & 0xF) * one16th + x_half, ((texture >> 16) & 0xF) * one16th + y_half, (texture & 0xFFF));

	int blockLight = (light & 0xF);
	int skyLight = internal_light - (15 - ((light >> 4) & 0xF));
	int shadow = 15 - max(blockLight, skyLight);
	int cornerLight = ((light >> 12) & 0x3);
	int faceLight = 100 - ((((light >> 8) & 0x7) > 0) ? 8 + (((light >> 8) & 0x7) << 2) : 0);
	FaceShadow = max(0, (max(10, faceLight - 7 * shadow) - 17 * cornerLight) * 0.01f);
	FaceShadow = mix(min_brightness, 1, FaceShadow);

	zDist = gl_Position.z;
}
