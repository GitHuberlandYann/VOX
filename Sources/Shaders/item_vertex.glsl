#version 150 core

/* specifications is packed
 * 0x00000FFF texture index in [0:4095]
 * 0x0000F000 x texture coord in [0:15]
 * 0x000F0000 y texture coord in [0:15]
 * 0x00100000 x texture half-pixel correction (1 << 20)
 * 0x00200000 y texture half-pixel correction (1 << 21)
 * 0x7 << 22 depth / 60
 * 0xF0000000 block light
*/
in int specifications;
in ivec2 position;

uniform int win_width;
uniform int win_height;

out vec3 TexCoords;
out float FaceShadow;

const float one16th = 0.0625f;
const float one256th = 0.00390625f;
const float half_pxl = 0.0001220703125f;

void main()
{
	gl_Position = vec4((2.0 * position.x) / win_width - 1.0, -((2.0 * position.y) / win_height - 1.0), ((specifications >> 22) & 0x3) * 60 * one256th * 0.01f, 1.0);
	
	float x_half = (((specifications & (1 << 20)) == 0) ? half_pxl : one16th - half_pxl);
	float y_half = (((specifications & (1 << 21)) == 0) ? half_pxl : one16th - half_pxl);
	TexCoords = vec3(((specifications >> 12) & 0xF) * one16th + x_half, ((specifications >> 16) & 0xF) * one16th + y_half, (specifications & 0xFFF));

	int blockLight = ((specifications >> 24) & 0xF);
	int shadow = 15 - blockLight;
	FaceShadow = max(0.05, max(10, 100 - 7 * shadow) / 100.0f);
}
