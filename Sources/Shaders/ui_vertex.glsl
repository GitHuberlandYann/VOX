#version 150 core

/*
 * 0xFF x texture
 * 0xFF00 y texture
 * 0x10000 x offset
 * 0x20000 y offset
 * 0x3F << 18 atlas
 * 0xFF000000 depth
 */
in int specifications;
in ivec2 position;

uniform int win_width;
uniform int win_height;

out vec3 TexCoords;

const float one256th = 0.00390625f;
const float half_pxl = 0.0001220703125f;

void main()
{
	gl_Position = vec4((2.0f * position.x) / win_width - 1.0f, -((2.0f * position.y) / win_height - 1.0f), ((specifications >> 24) & 0xFF) * one256th * 0.01f, 1.0f);

	TexCoords = vec3((specifications & 0xFF)       * one256th + (((specifications & 0x10000) == 0x10000) ? one256th - half_pxl : half_pxl),
					((specifications >> 8) & 0xFF) * one256th + (((specifications & 0x20000) == 0x20000) ? one256th - half_pxl : half_pxl),
					(specifications >> 18) & 0x3F);
}
