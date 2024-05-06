#version 150 core

in vec2 TexCoords;
in float FaceShadow;
in float zDist;

uniform float fogDist;

out vec4 outColor;

uniform sampler2D blockAtlas;

void main()
{
	// if (gl_FrontFacing) // this works but doesn't gain fps
	// 	discard ;

	outColor = texture(blockAtlas, TexCoords);
	if(outColor.a < 0.01f) {
		//outColor = vec4(1, 0, 0, 1);
		//return ;
		discard ;
	}

	outColor *= vec4(FaceShadow, FaceShadow, FaceShadow, 1.0f - smoothstep(fogDist * 0.5f, fogDist, zDist));
}
