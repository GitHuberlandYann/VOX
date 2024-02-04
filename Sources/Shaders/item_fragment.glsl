#version 150 core

in vec2 Texcoord;
in float FaceShadow;

out vec4 outColor;

uniform sampler2D blockAtlas;

void main()
{
	outColor = texture(blockAtlas, Texcoord);
	if(outColor.a < 0.01) {
		discard ;
	}
	outColor *= vec4(FaceShadow, FaceShadow, FaceShadow, 1.0);
}
