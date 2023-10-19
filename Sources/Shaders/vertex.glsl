#version 150 core

// in float face_num;
in int specifications; // 0xF = x coord, 0xF0 = y coord, 0xF00 x break, 0xF000 y break, 0xFF0000 shading [0:100]
// in int block_type;
// in int break_frame;
// in int adj_blocks;
in ivec3 position;
// in vec3 color;
// in vec2 texcoord;
// in vec3 normal;

// uniform int color_mode;
// uniform int tex_index;
// uniform int last_tex;
// uniform int invert_color;
// uniform int use_light;

// uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
// uniform mat4 scale;

// // uniform vec3 camPos;
// uniform vec3 lightPos;
// uniform vec3 lightColor;

// out vec3 Color;
// out vec2 Texcoord;
// out vec3 CamPos;
// out vec3 LightPos;
// out vec3 LightColor;
// out vec3 FragPos;
// out vec3 Normal;

// flat out int UseLight;
// flat out int UseTex;
// flat out int Tex_index;
// flat out int Invert;

// out Prim
// {
//     int Block_type;
// 	int Break_frame;
// 	int Adj_blocks;
// };

out vec2 Texcoord;
out vec2 Breakcoord;
out float FaceShadow;

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

	gl_Position = proj * view * vec4(position, 1.0);
	Texcoord = vec2((specifications & 0xF) / 16.0f, ((specifications >> 4) & 0xF) / 16.0f);
	Breakcoord = vec2((15.0f + ((specifications >> 8) & 0xF)) / 16.0f, ((specifications >> 12) & 0xF) / 16.0f);
	FaceShadow = ((specifications >> 16) & 0xFF) / 100.0f;
	// 0xF = x coord, 0xF0 = y coord, 0xF00 x break, 0xF000 y break, 0xFF0000 shading [0:100]

	// gl_Position = vec4(position, 1.0);
	// Block_type = (specifications & 0xFF);
	// Break_frame = ((specifications >> 8) & 0xFF);
	// Adj_blocks = ((specifications >> 16) & 0xFF);
	// Block_type = block_type;
	// Break_frame = break_frame;
	// Adj_blocks = adj_blocks;
}
