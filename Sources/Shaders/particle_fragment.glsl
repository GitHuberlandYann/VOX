#version 150 core

in vec2 Texcoord;
in float FaceShadow;
flat in int Atlas;

out vec4 outColor;

uniform sampler2D blockAtlas;
uniform sampler2D particleAtlas;

void main()
{
	if (Atlas == 0) {
		outColor = texture(blockAtlas, Texcoord);
	} else if (Atlas == 1) {
		outColor = texture(particleAtlas, Texcoord);
	} else {
		discard ;
	}
	if(outColor.a < 0.01) {
		discard ;
	}
	outColor *= vec4(FaceShadow, FaceShadow, FaceShadow, 1.0);
}
