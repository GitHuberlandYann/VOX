#version 150 core

/*
 * 0xF x texture
 * 0xF0 y texture
 * 0x100 x offset
 * 0x200 y offset
 * 0xFF000000 depth
 */
in int specifications;
/*
 * 0xFF000000 Alpha
 * 0x00FF0000 Red
 * 0x0000FF00 Green
 * 0x000000FF Blue
 */
in uint color;
in ivec2 position;

uniform int win_width;
uniform int win_height;

out vec2 TexCoords;
out vec4 Color;

const float one256th = 0.00390625f;
const float one16th = 0.0625f;
const float one8th = 0.125f;
const float half_pxl = 0.0001220703125f;

void main()
{
	gl_Position = vec4((2.0f * position.x) / win_width - 1.0f, -((2.0f * position.y) / win_height - 1.0f), ((specifications >> 24) & 0xFF) * one256th * 0.01f, 1.0f);

	TexCoords = vec2((specifications & 0xF)       * one16th + (((specifications & 0x100) == 0x100) ? one16th - half_pxl : half_pxl),
					((specifications >> 4) & 0xF) * one8th  + (((specifications & 0x200) == 0x200) ? one8th - half_pxl : half_pxl));

	Color = vec4((color >> 16) & 0xFFU, (color >> 8) & 0xFFU, color & 0xFFU, (color >> 24) & 0xFFU) / 255.0f;
}
