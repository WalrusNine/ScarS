#version 330

// Define struct for Directional Light
struct directional_light {
	vec3 color;
	vec3 direction;
	float ambient_intensity;
};

// Define struct for SpotLight
struct spotlight {
	vec3 color;
	vec3 position;
	vec3 direction;

	int isOn;

	float coneAngle, coneCosine;

	float linearAtt;
};

// IN variables
smooth in vec2 s_coord;
smooth in vec4 s_color;
smooth in vec3 s_normal;
smooth in vec3 s_position;

// Uniforms
uniform sampler2D g_sampler;

uniform directional_light sunlight;

uniform spotlight left_spotlight;
uniform spotlight right_spotlight;

vec4 get_spotlight_color (const spotlight s, vec3 world_pos) {
	// If flashlight isn't turned on, return no color
	if(s.isOn == 0) return vec4(0.0, 0.0, 0.0, 0.0);

	// Distance from fragment's position
	float dist = distance(world_pos, s.position);

	// Get direction vector to fragment
	vec3 direction = world_pos - s.position;
	direction = normalize(direction);

	// Cosine between spotlight direction and directional vector to fragment
	float cosine = dot(s.direction, direction);

	// Difference between max cosine and current cosine
	float diff = 1.0 - s.coneCosine;

	// This is how strong light is depending whether its nearer to the center of
	// cone or nearer to its borders (onway factor in article), clamp to 0.0 and 1.0
	float factor = clamp((cosine - s.coneCosine) / diff, 0.0, 1.0);

	// If we're inside cone, calculate color
	if(cosine > s.coneCosine)
		return vec4(s.color, 1.0) * factor / (dist * s.linearAtt);

	// No color otherwise
	return vec4(0.0, 0.0, 0.0, 0.0);
}

void main()
{
	// Get color from texture
	vec4 texture_color = texture2D(g_sampler, s_coord);

	// Calculate directional light
	// Diffuse = intensity depending on (normal,direction) angle
	float diffuse_intensity = max (0.0, dot(normalize(s_normal), -sunlight.direction));
	vec4 dir_light = vec4 (sunlight.color*(sunlight.ambient_intensity + diffuse_intensity), 1.0);

	// Calculate Spotlight
	vec4 ls_color = get_spotlight_color(left_spotlight, s_position);
	vec4 rs_color = get_spotlight_color(right_spotlight, s_position);
	vec4 spotlight_color = ls_color + rs_color;

	// Final color
	gl_FragColor = texture_color * s_color * (dir_light + spotlight_color);
}