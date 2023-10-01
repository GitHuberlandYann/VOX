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
	int Break_frame;
    int Adj_blocks;
} vsData[];

// out vec3 Color;
out vec2 Texcoord;
out vec3 Breakcoord;
out float FaceShadow;

const vec2 top = vec2(1.0f / 16.0f, 0.0f);
const vec2 bottom = vec2(2.0f / 16.0f, 0.0f);

const vec2 top_left = vec2(0.0f, 0.0f);
const vec2 top_right = vec2(0.99f / 16.0f, 0.0f);
const vec2 bottom_left = vec2(0.0f, 0.99f / 16.0f);
const vec2 bottom_right = vec2(0.99f / 16.0f, 0.99f / 16.0f);

void emitFace( vec4 v0, vec4 v1, vec4 v2, vec4 v3, vec2 start )
{
	vec2 breakStart = vec2(15.0f / 16.0f, vsData[0].Break_frame / 16.0f);
    gl_Position = v0;
	Texcoord = start + top_left;
	Breakcoord = vec3(breakStart + top_left, vsData[0].Break_frame);
    EmitVertex();
    gl_Position = v1;
	Texcoord = start + top_right;
	Breakcoord = vec3(breakStart + top_right, vsData[0].Break_frame);
    EmitVertex();
    gl_Position = v2;
	Texcoord = start + bottom_left;
	Breakcoord = vec3(breakStart + bottom_left, vsData[0].Break_frame);
    EmitVertex();

    EndPrimitive();

    gl_Position = v1;
	Texcoord = start + top_right;
	Breakcoord = vec3(breakStart + top_right, vsData[0].Break_frame);
    EmitVertex();
    gl_Position = v2; // might want to put this back in v1 - v3 - v2 disposition ..
	Texcoord = start + bottom_left;
	Breakcoord = vec3(breakStart + bottom_left, vsData[0].Break_frame);
    EmitVertex();
    gl_Position = v3;
	Texcoord = start + bottom_right;
	Breakcoord = vec3(breakStart + bottom_right, vsData[0].Break_frame);
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

	vec2 start = vec2(6.0f / 16.0f, (vsData[0].Block_type - 48.0f) / 16.0f);
	FaceShadow = 1.0;
	emitFace(v1, v4, v3, v6, start);
	emitFace(v0, v5, v2, v7, start);
}

void default_block( bool ores )
{
	vec4 v0 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0));
    vec4 v1 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0));
    vec4 v2 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0));
    vec4 v3 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0));

    vec4 v4 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0));
    vec4 v5 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 1.0, 1.0, 0.0));
    vec4 v6 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0));
    vec4 v7 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0));

	vec2 start = (ores) ? vec2(5.0f / 16.0f, (vsData[0].Block_type - 32.0f) / 16.0f)
						: vec2(4.0f / 16.0f, (vsData[0].Block_type - 16.0f) / 16.0f);
	
	if ((vsData[0].Adj_blocks & (1 << 4)) == 0) {
		FaceShadow = 1.0;
		emitFace(v4, v5, v0, v1, start);
	}
	if ((vsData[0].Adj_blocks & (1 << 0)) == 0) {
		FaceShadow = 0.92;
		emitFace(v0, v1, v2, v3, start);
	}
	if ((vsData[0].Adj_blocks & (1 << 1)) == 0) {
		FaceShadow = 0.88;
		emitFace(v5, v4, v7, v6, start);
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
		emitFace(v2, v3, v6, v7, start);
	}
}

void oriented_block( void )
{
	vec4 v0 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0));
    vec4 v1 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0));
    vec4 v2 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0));
    vec4 v3 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0));

    vec4 v4 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0));
    vec4 v5 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 1.0, 1.0, 0.0));
    vec4 v6 = proj * view * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0));
    vec4 v7 = proj * view * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0));

	vec2 start = vec2(0.0f, vsData[0].Block_type / 16.0f);
	
	if ((vsData[0].Adj_blocks & (1 << 4)) == 0) {
		FaceShadow = 1.0;
		emitFace(v4, v5, v0, v1, start + top);
	}
	if ((vsData[0].Adj_blocks & (1 << 0)) == 0) {
		FaceShadow = 0.92;
		((vsData[0].Adj_blocks & (1 << 6)) == 0)
			? emitFace(v0, v1, v2, v3, start)
			: emitFace(v0, v1, v2, v3, start + bottom); // use 'bottom' texture as front texture
	}
	if ((vsData[0].Adj_blocks & (1 << 1)) == 0) {
		FaceShadow = 0.88;
		((vsData[0].Adj_blocks & (1 << 7)) == 0)
			? emitFace(v5, v4, v7, v6, start)
			: emitFace(v5, v4, v7, v6, start + bottom);
	}
	if ((vsData[0].Adj_blocks & (1 << 2)) == 0) {
		FaceShadow = 0.84;
		((vsData[0].Adj_blocks & (1 << 8)) == 0)
			? emitFace(v4, v0, v6, v2, start)
			: emitFace(v4, v0, v6, v2, start + bottom);
	}
	if ((vsData[0].Adj_blocks & (1 << 3)) == 0) {
		FaceShadow = 0.80;
		((vsData[0].Adj_blocks & (1 << 9)) == 0)
			? emitFace(v1, v5, v3, v7, start)
			: emitFace(v1, v5, v3, v7, start + bottom);
	}
	if ((vsData[0].Adj_blocks & (1 << 5)) == 0) {
		FaceShadow = 0.74;
		emitFace(v2, v3, v6, v7, start + top); // use top texture as bottom too
	}
}

void main( void )
{
	if (vsData[0].Block_type == 0) {
		return ;
	} else if ((vsData[0].Block_type & 48) == 48) { // flower
		cross_image();
		return ;
	} else if (vsData[0].Block_type >= 16) { // same texture on 4 faces
		default_block((vsData[0].Block_type & 32) == 32);
		return ;
	} else if (vsData[0].Block_type >= 8) {
		oriented_block();
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

	vec2 start = vec2(0.0f, vsData[0].Block_type / 16.0f);
	
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
		emitFace(v5, v4, v7, v6, start);
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
		emitFace(v2, v3, v6, v7, start + bottom);
	}
}
