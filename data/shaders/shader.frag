#version 330

// Define struct for Directional Light
struct directional_light {
	vec3 color;
	vec3 direction;
	float ambient_intensity;
};

// IN variables
smooth in vec2 s_coord;
smooth in vec4 s_color;
smooth in vec3 s_normal;

// Uniforms
uniform sampler2D gSampler;

// Directional Light
uniform directional_light sunlight;

void main()
{
	// Get color from texture
	vec4 texture_color = texture2D(gSampler, s_coord);

	// Calculate directional light
	// Diffuse = intensity depending on (normal,direction) angle
	float diffuse_intensity = max (0.0, dot(normalize(s_normal), -sunlight.direction));
	vec4 dir_light = vec4 (sunlight.color*(sunlight.ambient_intensity + diffuse_intensity), 1.0);

	// Final color
	gl_FragColor = texture_color * s_color * dir_light;
}