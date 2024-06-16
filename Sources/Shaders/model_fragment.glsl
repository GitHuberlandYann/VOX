#version 150 core

in vec3 TexCoords;
in float FaceShadow;
in float hurt; // if entity is hurt, we reduce green and blue channels to give it a redish color

out vec4 outColor;

uniform sampler2DArray textures;

void main()
{
	outColor = texture(textures, TexCoords);
	if(outColor.a < 0.01f) {
		discard ;
	}
	outColor *= vec4(FaceShadow, FaceShadow * hurt, FaceShadow * hurt, 1.0f);
}
