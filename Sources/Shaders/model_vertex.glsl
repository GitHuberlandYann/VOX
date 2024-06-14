#version 150 core

/* specifications is packed
 * 0x3F = x coords in texture atlas
 * 0x3F << 6 = y coords in texture atlas
 * 0x1F << 12 texture Atlas
 * 0x1 << 17 x texture half-pixel correction
 * 0x1 << 18 y texture half-pixel correction
 * 0x7 << 19
 * 0x3 << 22
 * 0xF000000 block light
 * 0xF0000000 sky light
*/
in int specifications;
in vec3 position;

uniform mat4 view;
uniform mat4 proj;
uniform int internal_light;
uniform float min_brightness;

out vec3 TexCoords;
out float FaceShadow;

const float one64th = 0.015625f;

void main()
{
	gl_Position = proj * view * vec4(position, 1.0);

	float x_half = (((specifications & (1 << 17)) == 0) ? 0.0001220703125 : -0.0001220703125);
	float y_half = (((specifications & (1 << 18)) == 0) ? 0.0001220703125 : -0.0001220703125);
	TexCoords = vec3((specifications & 0x3F) * one64th + x_half, ((specifications >> 6) & 0x3F) * one64th + y_half, (specifications >> 12) & 0x1F);

	int blockLight = ((specifications >> 24) & 0xF);
	int skyLight = internal_light - (15 - ((specifications >> 28) & 0xF));
	int shadow = 15 - max(blockLight, skyLight);
	FaceShadow = max(0, (100 - 7 * shadow) * 0.01f);
	FaceShadow = mix(min_brightness, 1, FaceShadow);
}
