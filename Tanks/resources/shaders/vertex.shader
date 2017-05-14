#version 430 core

out vec2 UV;

layout(location = 0) uniform mat3 MVP;
layout(location = 1) uniform vec2 uv;
layout(location = 2) uniform vec2 position;

void main()
{
	UV = vec2(uv[gl_VertexID / 2], uv[gl_VertexID % 2]);

	gl_Position = vec4(
		(MVP * vec3(position[gl_VertexID / 2], position[gl_VertexID % 2], 1.0)).xy,
		0.0f, 
		1.0f
	);
}