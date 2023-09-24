#version 150 core

// in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D blockAtlas;
uniform sampler2D uiAtlas;

void main()
{
	// outColor = vec4(Color, 1.0);
	outColor = texture(uiAtlas, Texcoord);
	if(outColor.a < 0.01) {
		discard;
	}
}
