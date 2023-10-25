#version 150 core

in ivec4 position;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 color;
uniform int animFrame;

out vec3 Color;
out vec2 TexCoord;
flat out int Atlas;
out float zDist;

void main()
{
	int level = position.w & 0xF;
	gl_Position = proj * view * vec4(position.xy, position.z - (level / 8.0f), 1.0);
	Color = color;
	Atlas = (position.w >> 8) & 3;
	if (Atlas != 0) {
		int shiftX = position.w >> 10;
		int shiftY = position.w >> 11;
		int shiftX2 = position.w >> 12;
		int shiftY2 = position.w >> 13;
		TexCoord = vec2((shiftX & 1) + (shiftX2 & 1) * 0.5f, (animFrame + (shiftY & 1) + (shiftY2 & 1) * 0.5f) / 32.0f);
	}
	zDist = gl_Position.z;
}
