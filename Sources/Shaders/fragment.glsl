#version 150 core

// in vec3 Color;
in vec2 Texcoord;
in float FaceShadow;
// // in vec3 CamPos;
// in vec3 LightPos;
// in vec3 LightColor;
// in vec3 FragPos;
// in vec3 Normal;

// flat in int UseLight;
// flat in int UseTex;
// flat in int Tex_index;
// flat in int Invert;

out vec4 outColor;

uniform sampler2D tex0;

void main()
{
	outColor = texture(tex0, Texcoord) * vec4(FaceShadow, FaceShadow, FaceShadow, 1.0);
	
	// if (Invert == 1) {
	// 	outColor = vec4(1.0, 1.0, 1.0, 2.0) - outColor;
	// } else if (Invert == 2) {
	// 	float avg = 0.2126 * outColor.r + 0.7152 * outColor.g + 0.0722 * outColor.b;
	// 	outColor = vec4(avg, avg, avg, 1.0);
	// }

	// if (UseLight == 1) {
	// 	float ambientStrength = 0.1;
	// 	vec3 ambient = ambientStrength * LightColor;

	// 	vec3 norm = normalize(Normal);
	// 	vec3 lightDir = normalize(LightPos - FragPos);
	// 	float diff = max(dot(norm, lightDir), 0.0);
	// 	vec3 diffuse = diff * LightColor;

		// // float specularStrength = 0.5;
		// // vec3 camDir = normalize(CamPos - FragPos);
		// // vec3 reflectDir = reflect(-lightDir, norm);
		// // float spec = pow(max(dot(camDir, reflectDir), 0.0), 32);
		// // vec3 specular = specularStrength * spec * LightColor;  

		// // vec3 tmpColor = (ambient + diffuse + specular) * vec3(outColor);
		// vec3 tmpColor = (ambient + diffuse) * vec3(outColor);
		// outColor = vec4(tmpColor, 1.0);
	// }
	// outColor = vec4(Color, 1.0);
}
