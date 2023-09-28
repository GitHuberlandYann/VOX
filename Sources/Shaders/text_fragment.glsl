#version 150 core

in vec2 Texcoord;
in vec4 Color;

out vec4 outColor;

uniform sampler2D asciiAtlas;

void main()
{
	outColor = texture(asciiAtlas, Texcoord);
	if(outColor.a < 0.01) {
		discard;
	}
	outColor = outColor * Color;
}
