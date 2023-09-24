#version 150 core

in ivec4 pos;
in ivec4 textcoord;

uniform int window_width;
uniform int window_height;

out Prim
{
    out vec2 Size;
	out vec4 textCoord;
};

void main()
{
	gl_Position = vec4((2.0 * pos.x) / window_width - 1.0, -((2.0 * pos.y) / window_height - 1.0), 0.0, 1.0);
	Size = vec2((2.0 * pos.z) / window_width, -(2.0 * pos.w) / window_height);
	textCoord = textcoord;
}
