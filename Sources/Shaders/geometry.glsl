#version 150 core

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

uniform mat4 model;
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

vec2 top_left = vec2(0.0f, 0.0f);
vec2 top_right = vec2(1.0f, 0.0f);
vec2 bottom_left = vec2(0.0f, 1.0f);
vec2 bottom_right = vec2(1.0f, 1.0f);

void emitFace(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
{
    gl_Position = v0;
	Texcoord = top_left;
    EmitVertex();
    gl_Position = v1;
	Texcoord = top_right;
    EmitVertex();
    gl_Position = v2;
	Texcoord = bottom_left;
    EmitVertex();

    gl_Position = v1;
	Texcoord = top_right;
    EmitVertex();
    gl_Position = v3;
	Texcoord = bottom_right;
    EmitVertex();
    gl_Position = v2;
	Texcoord = bottom_left;
    EmitVertex();

    EndPrimitive();
}

void main()
{
    vec4 v0 = proj * view * model * (gl_in[0].gl_Position + vec4(-0.5, -0.5, 0.5, 0.0));
    vec4 v1 = proj * view * model * (gl_in[0].gl_Position + vec4(0.5, -0.5, 0.5, 0.0));
    vec4 v2 = proj * view * model * (gl_in[0].gl_Position + vec4(-0.5, -0.5, -0.5, 0.0));
    vec4 v3 = proj * view * model * (gl_in[0].gl_Position + vec4(0.5, -0.5, -0.5, 0.0));

    vec4 v4 = proj * view * model * (gl_in[0].gl_Position + vec4(-0.5, 0.5, 0.5, 0.0));
    vec4 v5 = proj * view * model * (gl_in[0].gl_Position + vec4(0.5, 0.5, 0.5, 0.0));
    vec4 v6 = proj * view * model * (gl_in[0].gl_Position + vec4(-0.5, 0.5, -0.5, 0.0));
    vec4 v7 = proj * view * model * (gl_in[0].gl_Position + vec4(0.5, 0.5, -0.5, 0.0));

    if ((vsData[0].Adj_blocks & 1) == 0) {
        emitFace(v0, v1, v2, v3);
    }
    if ((vsData[0].Adj_blocks & 2) == 0) {
        emitFace(v4, v5, v6, v7);
    }
    if ((vsData[0].Adj_blocks & 4) == 0) {
        emitFace(v4, v0, v6, v2);
    }
    if ((vsData[0].Adj_blocks & 8) == 0) {
        emitFace(v1, v5, v3, v7);
    }
    if ((vsData[0].Adj_blocks & 16) == 0) {
        emitFace(v4, v5, v0, v1);
    }
    if ((vsData[0].Adj_blocks & 32) == 0) {
        emitFace(v6, v7, v2, v3);
    }
}
