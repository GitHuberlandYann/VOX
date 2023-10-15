#version 150 core

in ivec2 position;
in int size;
in int character;
in int color;

uniform int window_width;
uniform int window_height;

out Prim
{
    out vec2 Size;
	out int Character;
	out int Color;
};

void main()
{
	gl_Position = vec4((2.0 * position.x) / window_width - 1.0, -((2.0 * position.y) / window_height - 1.0), 0.0, 1.0);
	Size = vec2((2.0 * size) / window_width, -(2.0 * size) / window_height);
	Character = character;
	Color = color;
}
