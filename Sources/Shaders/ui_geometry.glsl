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

const float half_texel = 0.0001220703125;

void emitFace( vec4 v0, vec4 v1, vec4 v2, vec4 v3 )
{
	vec2 top_left     = vec2(vsData[0].textCoord.x / 256.0f, vsData[0].textCoord.y / 256.0f);
    vec2 top_right    = top_left + vec2(vsData[0].textCoord.z / 256.0f, 0.0f);
    vec2 bottom_left  = top_left + vec2(0.0f, vsData[0].textCoord.w / 256.0f);
    vec2 bottom_right = top_left + vec2(vsData[0].textCoord.z / 256.0f, vsData[0].textCoord.w / 256.0f);
	
	gl_Position = v0;
	Texcoord = top_left + vec2(half_texel, half_texel);
    EmitVertex();
    gl_Position = v1;
	Texcoord = top_right + vec2(-half_texel, half_texel);
    EmitVertex();
    gl_Position = v2;
	Texcoord = bottom_left + vec2(half_texel, -half_texel);
    EmitVertex();

    gl_Position = v1;
	Texcoord = top_right + vec2(-half_texel, half_texel);
    EmitVertex();
    gl_Position = v2;
	Texcoord = bottom_left + vec2(half_texel, -half_texel);
    EmitVertex();
    gl_Position = v3;
	Texcoord = bottom_right + vec2(-half_texel, -half_texel);
    EmitVertex();

    EndPrimitive();
}

void main()
{
	Atlas = vsData[0].Atlas;

	vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[0].gl_Position + vec4(vsData[0].Size.x, 0.0, 0.0, 0.0);
    vec4 v2 = gl_in[0].gl_Position + vec4(0.0, vsData[0].Size.y, 0.0, 0.0);
    vec4 v3 = gl_in[0].gl_Position + vec4(vsData[0].Size.x, vsData[0].Size.y, 0.0, 0.0);

	emitFace(v0, v1, v2, v3);
}
