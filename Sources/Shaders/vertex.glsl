#version 150 core

/* specifications is packed
 * 0xFF = x coord in texture atlas
 * 0xFF00 = y coord in texture atlas
 * 0x1 << 16 unused
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

out vec2 TexCoords;
out float FaceShadow;
out float zDist;

const float one256th = 0.00390625f;
const float half_pxl = 0.0001220703125f;

void main()
{
	gl_Position = proj * view * vec4(position, 1.0);

	float x_half = (((specifications & (1 << 17)) == 0) ? half_pxl : -half_pxl);
	float y_half = (((specifications & (1 << 18)) == 0) ? half_pxl : -half_pxl);
	TexCoords = vec2((specifications & 0xFF) * one256th + x_half, ((specifications >> 8) & 0xFF) * one256th + y_half);

	int blockLight = ((specifications >> 24) & 0xF);
	int skyLight = internal_light - (15 - ((specifications >> 28) & 0xF));
	int shadow = 15 - max(blockLight, skyLight);
	int cornerLight = ((specifications >> 22) & 3);
	int faceLight = 100 - ((((specifications >> 19) & 0x7) > 0) ? 8 + (((specifications >> 19) & 0x7) << 2) : 0);
	FaceShadow = max(0, (max(10, faceLight - 7 * shadow) - 17 * cornerLight) * 0.01f);
	FaceShadow = mix(min_brightness, 1, FaceShadow);
	// FaceShadow = max(0.05f, (max(10, faceLight - 7 * shadow) - 17 * cornerLight) * 0.01f);
	// FaceShadow = mix(min_brightness, 1, (FaceShadow - 0.05f) / (0.95f));

	zDist = gl_Position.z;
}
