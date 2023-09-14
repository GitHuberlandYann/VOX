#version 150 core

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 scale;

// out vec3 Color;
out vec2 Texcoord;

void main()
{
    vec2 top_left = vec2(0.0f, 1.0f); // y value is upside down
    vec2 top_right = vec2(1.0f, 1.0f);
    vec2 bottom_left = vec2(0.0f, 0.0f);
    vec2 bottom_right = vec2(1.0f, 0.0f);

    vec4 v0 = proj * view * model * scale * (gl_in[0].gl_Position + vec4(-0.2, 0.0, -0.2, 0.0));
    vec4 v1 = proj * view * model * scale * (gl_in[0].gl_Position + vec4(0.2, 0.0, -0.2, 0.0));
    vec4 v2 = proj * view * model * scale * (gl_in[0].gl_Position + vec4(-0.2, 0.0, 0.2, 0.0));
    vec4 v3 = proj * view * model * scale * (gl_in[0].gl_Position + vec4(0.2, 0.0, 0.2, 0.0));

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
