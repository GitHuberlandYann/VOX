#version 150 core

flat in int Atlas;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D blockAtlas;
uniform sampler2D uiAtlas;
uniform sampler2D containerAtlas;

void main()
{
	if (Atlas == 0) {
		outColor = texture(blockAtlas, Texcoord);
	} else if (Atlas == 1) {
		outColor = texture(uiAtlas, Texcoord);
	} else if (Atlas == 2) {
		outColor = texture(containerAtlas, Texcoord);	
	} else {
		outColor = vec4(1.0, 0.0, 0.0, 1.0); // red means trouble
	}
	if(outColor.a < 0.01) {
		discard ;
	}
}
