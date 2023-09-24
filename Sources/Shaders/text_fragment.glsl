#version 150 core

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D asciiAtlas;

void main()
{
	outColor = texture(asciiAtlas, Texcoord);
	if(outColor.a < 0.01) {
		discard;
	}
}
