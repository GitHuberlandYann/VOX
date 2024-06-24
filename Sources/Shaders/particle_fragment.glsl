#version 150 core

in vec2 TexCoords;
in float FaceShadow;

out vec4 outColor;

uniform sampler2D particleAtlas;

void main()
{
	outColor = texture(particleAtlas, TexCoords);
	if(outColor.a < 0.01) {
		discard ;
	}
	outColor *= vec4(FaceShadow, FaceShadow, FaceShadow, 1.0);
}
