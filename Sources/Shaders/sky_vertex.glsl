#version 150 core

/*
 * position.w is packed
 * 0xF level = float z offset from top, used to display water at several heights
 * 0xF0 unused
 * 0x300 Atlas index [No atlas used=clouds, Water still, Water flow, blockAtlas=glass]
 * 1 << 10 texture x shift by 1
 * 1 << 11 texture y shift by 1
 * 1 << 12 texture x shift by 0.5
 * 1 << 13 texture y shift by 0.5
 */
in ivec4 position;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 color;
uniform int animFrame;

out vec3 Color;
out vec2 TexCoord;
flat out int Atlas;
out float zDist;

const float one32th = 0.03125f;
const float one16th = 0.0625f;
const float one8th = 0.125f;

void main()
{
	int level = position.w & 0xF;
	gl_Position = proj * view * vec4(position.xy, position.z - (level * one8th), 1.0f);
	Color = color;
	Atlas = (position.w >> 8) & 0x3;
	if (Atlas == 3) {
		Color = vec3(1.0f, 1.0f, 1.0f);
		TexCoord = vec2((position.w >> 10) & 0x1, (position.w >> 11) & 0x1);
	} else if (Atlas != 0) {
		int shiftX = (position.w >> 10) & 0x1;
		int shiftY = (position.w >> 11) & 0x1;
		int shiftX2 = (position.w >> 12) & 0x1;
		int shiftY2 = (position.w >> 13) & 0x1;
		TexCoord = vec2(shiftX + shiftX2 * 0.5f, (animFrame + shiftY + shiftY2 * 0.5f) * one32th);
	}
	zDist = gl_Position.z;
}
