#version 150 core

in int atlas;
in ivec4 pos;
in ivec4 textcoord;

uniform int win_width;
uniform int win_height;

out Prim
{
	out int Atlas;
    out vec2 Size;
	out vec4 textCoord;
};

void main()
{
	gl_Position = vec4((2.0 * pos.x) / win_width - 1.0, -((2.0 * pos.y) / win_height - 1.0), 0.0, 1.0);
	Atlas = atlas;
	Size = vec2((2.0 * pos.z) / win_width, -(2.0 * pos.w) / win_height);
	textCoord = textcoord;
}
