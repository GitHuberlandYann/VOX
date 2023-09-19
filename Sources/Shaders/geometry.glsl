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
out float FaceShadow;

const vec2 top = vec2(1.0f / 16.0f, 0.0f);
const vec2 bottom = vec2(2.0f / 16.0f, 0.0f);

const vec2 top_left = vec2(0.0f, 0.0f);
const vec2 top_right = vec2(1.0f / 16.0f, 0.0f);
const vec2 bottom_left = vec2(0.0f, 1.0f / 16.0f);
const vec2 bottom_right = vec2(1.0f / 16.0f, 1.0f / 16.0f);

void emitFace( vec4 v0, vec4 v1, vec4 v2, vec4 v3, vec2 start )
{
    gl_Position = v0;
	Texcoord = start + top_left;
    EmitVertex();
    gl_Position = v1;
	Texcoord = start + top_right;
    EmitVertex();
    gl_Position = v2;
	Texcoord = start + bottom_left;
    EmitVertex();

    gl_Position = v1;
	Texcoord = start + top_right;
    EmitVertex();
    gl_Position = v3;
	Texcoord = start + bottom_right;
    EmitVertex();
    gl_Position = v2;
	Texcoord = start + bottom_left;
    EmitVertex();

    EndPrimitive();
}

void cross_image( void )
{
	vec4 v0 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0));
    vec4 v1 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0));
    vec4 v2 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0));
    vec4 v3 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0));

    vec4 v4 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0));
    vec4 v5 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 1.0, 1.0, 0.0));
    vec4 v6 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0));
    vec4 v7 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0));

	vec2 start = vec2(4.0f / 16.0f, (vsData[0].Block_type - 17.0f) / 16.0f);
	FaceShadow = 1.0;
	emitFace(v1, v4, v3, v6, start);
	emitFace(v0, v5, v2, v7, start);
}

void main()
{
	if (vsData[0].Block_type == 0) {
		return ;
	} else if (vsData[0].Block_type >= 17) { //flower
		cross_image();
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

	vec2 start = vec2(0.0f, (vsData[0].Block_type - 1.0f) / 16.0f);
	
	if ((vsData[0].Adj_blocks & (1 << 4)) == 0) {
		FaceShadow = 1.0;
		emitFace(v4, v5, v0, v1, start + top);
	}
	if ((vsData[0].Adj_blocks & (1 << 0)) == 0) {
		FaceShadow = 0.92;
		emitFace(v0, v1, v2, v3, start);
	}
	if ((vsData[0].Adj_blocks & (1 << 1)) == 0) {
		FaceShadow = 0.88;
		emitFace(v4, v5, v6, v7, start);
	}
	if ((vsData[0].Adj_blocks & (1 << 2)) == 0) {
		FaceShadow = 0.84;
		emitFace(v4, v0, v6, v2, start);
	}
	if ((vsData[0].Adj_blocks & (1 << 3)) == 0) {
		FaceShadow = 0.80;
		emitFace(v1, v5, v3, v7, start);
	}
	if ((vsData[0].Adj_blocks & (1 << 5)) == 0) {
		FaceShadow = 0.74;
		emitFace(v6, v7, v2, v3, start + bottom);
	}
}
