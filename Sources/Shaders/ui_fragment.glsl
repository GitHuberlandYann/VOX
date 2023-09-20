#version 150 core

in vec3 Color;
// in vec2 Texcoord;


out vec4 outColor;

// uniform sampler2D tex0;

void main()
{
	outColor = vec4(Color, 1.0);
	// if(outColor.a < 0.01) {
	// 	discard;
	// } else {
	// 	outColor = outColor * vec4(FaceShadow, FaceShadow, FaceShadow, 1.0);
	// }
}
