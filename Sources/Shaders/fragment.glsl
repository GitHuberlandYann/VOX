#version 150 core

in vec3 TexCoords;
in float FaceShadow;
in float zDist;

uniform float fogDist;

out vec4 outColor;

uniform sampler2DArray textures;

void main()
{
	outColor = texture(textures, TexCoords);
	if(outColor.a < 0.01f) {
		discard ;
	}

	outColor *= vec4(FaceShadow, FaceShadow, FaceShadow, 1.0f - smoothstep(fogDist * 0.5f, fogDist, zDist));
}
