#version 150 core

in vec3 Color;
in vec2 TexCoord;
flat in int Atlas;
in float zDist;

uniform float fogDist;

out vec4 outColor;

uniform sampler2D blockAtlas;
uniform sampler2D waterStill;
uniform sampler2D waterFlow;

void main()
{
	if (Atlas == 3) { // glass
		outColor = vec4(Color, 1.0f - smoothstep(fogDist * 0.5f, fogDist, zDist)) * texture(blockAtlas, TexCoord);
		return ;
	}

	outColor = vec4(Color, 0.7f * (1.0f - smoothstep(fogDist * 0.5f, fogDist, zDist)));
	if (Atlas == 0) { // sky
		return ;
	}
	// vec4 tex;
	// if (Atlas == 1) {
	// 	tex = texture(waterStill, TexCoord);
	// } else if (Atlas == 2) {
	// 	tex = texture(waterFlow, TexCoord);
	// }
	vec4 tex = (Atlas == 1) ? texture(waterStill, TexCoord)
							: texture(waterFlow,  TexCoord);
	// if (tex.a < 0.01) {
	// 	return ;
	// }
	outColor *= vec4(tex.rgb, 1.0);
}
