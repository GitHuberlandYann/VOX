#version 150 core

// in vec3 Color;
in vec2 Texcoord;
in vec2 Breakcoord;
in float FaceShadow;
in float zDist;
// // in vec3 CamPos;
// in vec3 LightPos;
// in vec3 LightColor;
// in vec3 FragPos;
// in vec3 Normal;

// flat in int UseLight;
// flat in int UseTex;
// flat in int Tex_index;
// flat in int Invert;

uniform float fogDist;

out vec4 outColor;

uniform sampler2D blockAtlas;

void main()
{
	// if (gl_FrontFacing) // this works but doesn't gain fps
	// 	discard ;
	outColor = texture(blockAtlas, Texcoord);
	if(outColor.a < 0.01) {
		discard ;
	}
	if (Breakcoord.y != 0) {
		vec4 break_ = texture(blockAtlas, Breakcoord);
		if (break_.a > 0.01) {
			outColor *= break_;
		}
	}
	float smoothies = 1.0 - smoothstep(fogDist / 2, fogDist, zDist);
	outColor.a *= smoothies;
	outColor *= vec4(FaceShadow, FaceShadow, FaceShadow, 1.0);
	
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
