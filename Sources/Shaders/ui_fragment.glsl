#version 150 core

in vec3 TexCoords;

out vec4 outColor;

uniform sampler2DArray textures;

void main()
{
	outColor = texture(textures, TexCoords);
	if(outColor.a < 0.01) {
		discard ;
	}
}
