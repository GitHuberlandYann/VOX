#version 150 core

in vec3 Color;
in vec2 TexCoord;
flat in int Atlas;
in float zDist;

uniform float fogDist;

out vec4 outColor;

uniform sampler2D waterStill;
uniform sampler2D waterFlow;

void main()
{
	outColor = vec4(Color, 0.7f);
	vec4 tex;
	if (Atlas == 0) {
		return ;
	} else if (Atlas == 1) {
		tex = texture(waterStill, TexCoord);
	} else if (Atlas == 2) {
		tex = texture(waterFlow, TexCoord);
	}
	if (tex.a < 0.01) {
		return ;
	}
	outColor *= vec4(tex.rgb, 1.0);
	outColor.a *= 1.0 - smoothstep(fogDist / 2, fogDist, zDist);
}
