#version 150 core

/*
 * 0xFF x texture
 * 0xFF00 y texture
 * 0x10000 x offset
 * 0x20000 y offset
 * rest atlas
 */
in int specifications;
in ivec2 position;

uniform int win_width;
uniform int win_height;

out vec2 TexCoords;
flat out int Atlas;

const float one256th = 0.00390625f;
const float half_pxl = 0.0001220703125f;

void main()
{
	gl_Position = vec4((2.0f * position.x) / win_width - 1.0f, -((2.0f * position.y) / win_height - 1.0f), 0.0f, 1.0f);

	TexCoords = vec2((specifications & 0xFF)       * one256th + (((specifications & 0x10000) == 0x10000) ? one256th - half_pxl : half_pxl),
					((specifications >> 8) & 0xFF) * one256th + (((specifications & 0x20000) == 0x20000) ? one256th - half_pxl : half_pxl));
					
	Atlas = (specifications >> 18);
}

/////


