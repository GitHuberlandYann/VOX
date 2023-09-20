#version 150 core

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

in Prim
{
    vec2 Size;
	vec3 Color;
} vsData[];

out vec3 Color;

// const vec2 top = vec2(1.0f / 16.0f, 0.0f);
// const vec2 bottom = vec2(2.0f / 16.0f, 0.0f);

// const vec2 top_left = vec2(0.0f, 0.0f);
// const vec2 top_right = vec2(1.0f / 16.0f, 0.0f);
// const vec2 bottom_left = vec2(0.0f, 1.0f / 16.0f);
// const vec2 bottom_right = vec2(1.0f / 16.0f, 1.0f / 16.0f);

void emitFace( vec4 v0, vec4 v1, vec4 v2, vec4 v3 )
{
    gl_Position = v0;
	// Texcoord = start + top_left;
    EmitVertex();
    gl_Position = v1;
	// Texcoord = start + top_right;
    EmitVertex();
    gl_Position = v2;
	// Texcoord = start + bottom_left;
    EmitVertex();

    gl_Position = v1;
	// Texcoord = start + top_right;
    EmitVertex();
    gl_Position = v3;
	// Texcoord = start + bottom_right;
    EmitVertex();
    gl_Position = v2;
	// Texcoord = start + bottom_left;
    EmitVertex();

    EndPrimitive();
}

void main()
{
	Color = vsData[0].Color;

	vec4 v0 = gl_in[0].gl_Position + vec4(0.0, -vsData[0].Size.y, 0.0, 0.0);
    vec4 v1 = gl_in[0].gl_Position + vec4(vsData[0].Size.x, -vsData[0].Size.y, 0.0, 0.0);
    vec4 v2 = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
    vec4 v3 = gl_in[0].gl_Position + vec4(vsData[0].Size.x, 0.0, 0.0, 0.0);

	emitFace(v0, v1, v2, v3);
}
