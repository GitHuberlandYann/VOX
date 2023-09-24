#version 150 core

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

in Prim
{
	int Atlas;
    vec2 Size;
	vec4 textCoord;
} vsData[];

flat out int Atlas;
out vec2 Texcoord;
// out vec3 Color;

// const vec2 top = vec2(1.0f / 16.0f, 0.0f);
// const vec2 bottom = vec2(2.0f / 16.0f, 0.0f);

// const vec2 top_left = vec2(0.0f, 0.0f);
// const vec2 top_right = vec2(1.0f / 16.0f, 0.0f);
// const vec2 bottom_left = vec2(0.0f, 1.0f / 16.0f);
// const vec2 bottom_right = vec2(1.0f / 16.0f, 1.0f / 16.0f);

void emitFace( vec4 v0, vec4 v1, vec4 v2, vec4 v3 )
{
	vec2 start = vec2(vsData[0].textCoord.x / 256.0f, vsData[0].textCoord.y / 256.0f);
    vec2 top_right = vec2(vsData[0].textCoord.z / 256.0f, 0.0f);
    vec2 bottom_left = vec2(0.0f, vsData[0].textCoord.w / 256.0f);
    vec2 bottom_right = vec2(vsData[0].textCoord.z / 256.0f, vsData[0].textCoord.w / 256.0f);
	
	gl_Position = v0;
	Texcoord = start;
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

void main()
{
	Atlas = vsData[0].Atlas;
	// Color = vec3(1.0f, 0.0f, 0.0f);
	vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[0].gl_Position + vec4(vsData[0].Size.x, 0.0, 0.0, 0.0);
    vec4 v2 = gl_in[0].gl_Position + vec4(0.0, vsData[0].Size.y, 0.0, 0.0);
    vec4 v3 = gl_in[0].gl_Position + vec4(vsData[0].Size.x, vsData[0].Size.y, 0.0, 0.0);

	emitFace(v0, v1, v2, v3);
}
