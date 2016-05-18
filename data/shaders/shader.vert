#version 330

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_coord;
layout (location = 3) in vec3 a_normal;
layout (location = 4) in vec4 a_spec;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

smooth out vec2 s_coord;
smooth out vec4 s_color;
smooth out vec3 s_normal;
smooth out vec3 s_position;

void main()
{
	// MVP
	vec4 world_position = modelMatrix * vec4(a_position, 1.0);
	gl_Position = projectionMatrix * viewMatrix * world_position;
	s_position = world_position.xyz;

	// Texture
	s_coord = a_coord;

	// Color
	s_color = a_color;

	// Normal
	vec4 temp_normal = normalMatrix * vec4(a_normal, 0.0);
	s_normal = temp_normal.xyz;
}