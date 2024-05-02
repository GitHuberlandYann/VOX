#version 150 core

in vec2 Texcoord;
in vec2 Breakcoord;
in float FaceShadow;
in float zDist;

uniform float fogDist;

out vec4 outColor;

uniform sampler2D blockAtlas;

void main()
{
	// if (gl_FrontFacing) // this works but doesn't gain fps
	// 	discard ;

	outColor = texture(blockAtlas, Texcoord);
	if(outColor.a < 0.01f) {
		discard ;
	}

	if (Breakcoord.y > 0.0625f) {
		vec4 break_ = texture(blockAtlas, Breakcoord);
		if (break_.a > 0.01f) {
			outColor *= break_;
		}
	}

	outColor *= vec4(FaceShadow, FaceShadow, FaceShadow, 1.0f - smoothstep(fogDist * 0.5f, fogDist, zDist));
}
