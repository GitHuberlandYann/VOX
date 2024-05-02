#version 150 core

in vec2 TexCoords;
in vec4 Color;

out vec4 outColor;

uniform sampler2D asciiAtlas;

void main()
{
	outColor = texture(asciiAtlas, TexCoords);
	if(outColor.a < 0.01) {
		discard;
	}
	outColor *= Color;
}
