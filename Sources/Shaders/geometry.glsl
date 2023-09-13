#version 150 core

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

// out vec3 Color;
out vec2 Texcoord;

void main()
{
    gl_Position = gl_in[0].gl_Position + vec4(-0.2, 0.2, 0.0, 0.0);
	Texcoord = vec2(0.0f, 0.0f);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.2, 0.2, 0.0, 0.0);
	Texcoord = vec2(1.0f, 0.0f);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.2, -0.2, 0.0, 0.0);
	Texcoord = vec2(1.0f, 1.0f);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.2, 0.2, 0.0, 0.0);
	Texcoord = vec2(0.0f, 0.0f);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.2, -0.2, 0.0, 0.0);
	Texcoord = vec2(0.0f, 1.0f);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.2, -0.2, 0.0, 0.0);
	Texcoord = vec2(1.0f, 1.0f);
    EmitVertex();

    EndPrimitive();
}
