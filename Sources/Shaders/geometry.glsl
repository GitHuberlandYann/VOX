#version 150 core

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

// uniform mat4 pv;
// uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
// uniform mat4 scale;

in Prim
{
    int Block_type;
    int Adj_blocks;
} vsData[];

// out vec3 Color;
out vec2 Texcoord;

// const vec2 top_left = vec2(0.0f, 0.0f);
// const vec2 top_right = vec2(1.0f, 0.0f);
// const vec2 bottom_left = vec2(0.0f, 1.0f);
// const vec2 bottom_right = vec2(1.0f, 1.0f);

const vec2 gb_0 = vec2(0.333587, 0.000975);
const vec2 gb_1 = vec2(0.666413, 0.000975);
const vec2 gb_2 = vec2(0.000761, 0.250594);
const vec2 gb_3 = vec2(0.333587, 0.250594);
const vec2 gb_4 = vec2(0.666413, 0.250594);
const vec2 gb_5 = vec2(0.999239, 0.250594);
const vec2 gb_6 = vec2(0.000761, 0.500214);
const vec2 gb_7 = vec2(0.333587, 0.500214);
const vec2 gb_8 = vec2(0.666413, 0.500214);
const vec2 gb_9 = vec2(0.999239, 0.500214);
const vec2 gb_10 = vec2(0.333587, 0.749833);
const vec2 gb_11 = vec2(0.666413, 0.749833);
const vec2 gb_12 = vec2(0.333587, 0.999452);
const vec2 gb_13 = vec2(0.666413, 0.999452);

void emitFace(vec4 v0, vec4 v1, vec4 v2, vec4 v3, vec2 topL, vec2 topR, vec2 bottomL, vec2 bottomR)
{
	// topL = top_left;
	// topR = top_right;
	// bottomL = bottom_left;
	// bottomR = bottom_right;
    gl_Position = v0;
	Texcoord = topL;
    EmitVertex();
    gl_Position = v1;
	Texcoord = topR;
    EmitVertex();
    gl_Position = v2;
	Texcoord = bottomL;
    EmitVertex();

    gl_Position = v1;
	Texcoord = topR;
    EmitVertex();
    gl_Position = v3;
	Texcoord = bottomR;
    EmitVertex();
    gl_Position = v2;
	Texcoord = bottomL;
    EmitVertex();

    EndPrimitive();
}

void main()
{
	if (vsData[0].Block_type == 0) {
		return ;
	}
    vec4 v0 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0));
    vec4 v1 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0));
    vec4 v2 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0));
    vec4 v3 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0));

    vec4 v4 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0));
    vec4 v5 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 1.0, 1.0, 0.0));
    vec4 v6 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0));
    vec4 v7 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0));

	if (vsData[0].Block_type == 1) {
		if ((vsData[0].Adj_blocks & (1 << 0)) == 0) {
			emitFace(v0, v1, v2, v3, gb_7, gb_8, gb_10, gb_11);
		}
		if ((vsData[0].Adj_blocks & (1 << 1)) == 0) {
			emitFace(v4, v5, v6, v7, gb_4, gb_3, gb_1, gb_0);
		}
		if ((vsData[0].Adj_blocks & (1 << 2)) == 0) {
			emitFace(v4, v0, v6, v2, gb_3, gb_7, gb_2, gb_6);
		}
		if ((vsData[0].Adj_blocks & (1 << 3)) == 0) {
			emitFace(v1, v5, v3, v7, gb_8, gb_4, gb_9, gb_5);
		}
		if ((vsData[0].Adj_blocks & (1 << 4)) == 0) {
			emitFace(v4, v5, v0, v1, gb_3, gb_4, gb_7, gb_8);
		}
		if ((vsData[0].Adj_blocks & (1 << 5)) == 0) {
			emitFace(v6, v7, v2, v3, gb_10, gb_11, gb_12, gb_13);
		}
	} else if (vsData[0].Block_type == 2) {
		if ((vsData[0].Adj_blocks & (1 << 0)) == 0) {
			emitFace(v0, v1, v2, v3, gb_10, gb_11, gb_12, gb_13);
		}
		if ((vsData[0].Adj_blocks & (1 << 1)) == 0) {
			emitFace(v4, v5, v6, v7, gb_10, gb_11, gb_12, gb_13);
		}
		if ((vsData[0].Adj_blocks & (1 << 2)) == 0) {
			emitFace(v4, v0, v6, v2, gb_10, gb_11, gb_12, gb_13);
		}
		if ((vsData[0].Adj_blocks & (1 << 3)) == 0) {
			emitFace(v1, v5, v3, v7, gb_10, gb_11, gb_12, gb_13);
		}
		if ((vsData[0].Adj_blocks & (1 << 4)) == 0) {
			emitFace(v4, v5, v0, v1, gb_10, gb_11, gb_12, gb_13);
		}
		if ((vsData[0].Adj_blocks & (1 << 5)) == 0) {
			emitFace(v6, v7, v2, v3, gb_10, gb_11, gb_12, gb_13);
		}
    }// else if (vsData[0].Block_type == 3) {
	// 	if ((vsData[0].Adj_blocks & (1 << 0)) == 0) {
	// 		emitFace(v0, v1, v2, v3, gb_3, gb_4, gb_7, gb_8);
	// 	}
	// 	if ((vsData[0].Adj_blocks & (1 << 1)) == 0) {
	// 		emitFace(v4, v5, v6, v7, gb_3, gb_4, gb_7, gb_8);
	// 	}
	// 	if ((vsData[0].Adj_blocks & (1 << 2)) == 0) {
	// 		emitFace(v4, v0, v6, v2, gb_3, gb_4, gb_7, gb_8);
	// 	}
	// 	if ((vsData[0].Adj_blocks & (1 << 3)) == 0) {
	// 		emitFace(v1, v5, v3, v7, gb_3, gb_4, gb_7, gb_8);
	// 	}
	// 	if ((vsData[0].Adj_blocks & (1 << 4)) == 0) {
	// 		emitFace(v4, v5, v0, v1, gb_3, gb_4, gb_7, gb_8);
	// 	}
	// 	if ((vsData[0].Adj_blocks & (1 << 5)) == 0) {
	// 		emitFace(v6, v7, v2, v3, gb_3, gb_4, gb_7, gb_8);
	// 	}
    // }
}
