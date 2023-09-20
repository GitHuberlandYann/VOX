#version 150 core

in vec2 position;
in vec2 size;
in vec3 color;


out Prim
{
    out vec2 Size;
	out vec3 Color;
};

// vec3 gray()
// {
// 	vec3 res;

// 	if (face_num < 0.5) {
// 		res = vec3(0.1, 0.1, 0.1);
// 	} else if (face_num < 1.5) {
// 		res = vec3(0.2, 0.2, 0.2);
// 	} else if (face_num < 2.5) {
// 		res = vec3(0.3, 0.3, 0.3);
// 	} else { // not supposed to happen
// 		res = vec3(1.0, 0.0, 0.0);
// 	}

// 	return (res);
// }

void main()
{
	// Texcoord = texcoord;
	// // CamPos = camPos;
	// LightPos = lightPos;
	// LightColor = lightColor;
	// FragPos = vec3(model * vec4(position, 1.0));
	// Normal = vec3(model * vec4(normal, 1.0));
	// UseLight = use_light;

	// UseTex = 0;
	// Tex_index = tex_index;
	// Invert = invert_color;

	// if (color_mode == 0) {
	// 	Color = vec3(1.0, 1.0, 1.0);
	// } else if (color_mode == 1) {
	// 	Color = color;
	// } else if (color_mode == 2) {
	// 	UseTex = 1;
	// } else if (color_mode == 3) {
	// 	// Texcoord = vec2(position.x + 0.5 + position.z / 10.0, -position.y + 0.5);
	// 	Texcoord = vec2((position.x + position.z) * 0.5 + 0.5, -position.y + 0.5);
	// 	UseTex = 1;
	// 	Tex_index = last_tex;
	// } else if (color_mode == 4) {
	// 	Color = gray();
	// } else if (color_mode == 5) {
	// 	Color = vec3((position.x + 0.5)  * 0.5, (position.y + 0.5)  * 0.5, (position.z + 0.5)  * 0.5);
	// } else { // not supposed to happen
	// 	Color = vec3(0.0, 0.0, 1.0);
	// }

	// gl_Position = proj * view * model * scale * vec4(position, 1.0);
	gl_Position = vec4(position, 0.0, 1.0);
	Size = size;
	Color = color;
}
