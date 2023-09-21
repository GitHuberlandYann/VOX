#version 150 core

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D tex0;

void main()
{
	outColor = texture(tex0, Texcoord);
	if(outColor.a < 0.01) {
		discard;
	}
}
