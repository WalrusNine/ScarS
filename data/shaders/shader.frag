#version 330

// In
smooth in vec2 s_coord;
smooth in vec4 s_color;
smooth in vec3 s_normal;
smooth in vec3 s_position;
smooth in vec4 eye_world_pos;

// Uniforms
uniform sampler2D g_sampler;

// Out
out vec4 FragColor;

/*********************
		LIGHT
*********************/

// Define struct for Directional Light
struct directional_light {
	vec3 color;
	vec3 direction;
	float ambient_intensity;
};

// Define struct for SpotLight
struct s_spotlight {
	vec3 color;
	vec3 position;
	vec3 direction;

	int isOn;

	float coneAngle;
	float coneCosine;

	float linearAtt;
};

uniform directional_light sunlight;
uniform s_spotlight spotlight;

// Spotlight
vec4 get_spotlight_color (const s_spotlight s, vec3 world_pos) {
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

/*********************
		SHADOW
*********************/

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

smooth in vec4 ShadowCoord;
uniform sampler2D shadowMap;

float GetVisibility(sampler2D ShadowMap, vec4 vShadowCoord) {
    float visibility = 1.0;
    float bias = 0.005;
 
    //vec4 shadowCoordinateWdivide = vec4(vShadowCoord.x/vShadowCoord.w, vShadowCoord.y/vShadowCoord.w, (vShadowCoord.z-bias)/vShadowCoord.w, 1.0) ;

    for (int i=0;i<4;i++) {
		int index = i;
		vec4 vShadowSmooth = vec4(vShadowCoord.x + poissonDisk[index].x/700.0, vShadowCoord.y + poissonDisk[index].y/700.0, (vShadowCoord.z-bias)/vShadowCoord.w, 1.0);
		float fSub = texture(ShadowMap, vShadowSmooth.st).r;

		visibility -= 0.1 * (1.0-fSub);
    }

    return visibility;
}

/*float GetVisibility(sampler2D ShadowMap, vec4 vShadowCoord) {
    float visibility = 1.0;
    float bias = 0.005;
 
    if (texture(ShadowMap, vShadowCoord.xy).z < vShadowCoord.z - bias) {
		visibility = 0.5;
	}

	return visibility;
}*/

/*********************
		FOG
*********************/

uniform struct FogParameters {
	vec4 fogColor;	// Fog color
	float start;	// This is only for linear fog
	float end;		// This is only for linear fog
	float density;	// For exp and exp2 equation
	
	int equation;	// 0 = linear, 1 = exp, 2 = exp2
} fogParams;

float getFogFactor(FogParameters params, float fogCoord) {
	float result = 0.0;

	if(params.equation == 0)
		result = (params.end-fogCoord)/(params.end-params.start);
	else if(params.equation == 1)
		result = exp(-params.density*fogCoord);
	else if(params.equation == 2)
		result = exp(-pow(params.density*fogCoord, 2.0));
		
	result = 1.0 - clamp(result, 0.0, 1.0);
	
	return result;
}

/*********************
		WATER
*********************/

//uniform sampler2D reflectionTexture;

/*********************
		MAIN
*********************/

void main()
{
	// Get color from texture
	vec4 texture_color = texture2D(g_sampler, s_coord);

	// Calculate directional light
	// Diffuse = intensity depending on (normal,direction) angle
	float diffuse_intensity = max (0.0, dot(normalize(s_normal), -sunlight.direction));
	vec4 dir_light = vec4 (sunlight.color*(sunlight.ambient_intensity + diffuse_intensity), 1.0);

	// Calculate Spotlight
	vec4 spotlight_color = get_spotlight_color(spotlight, s_position);
	
	// Calculate Shadow
	float visibility = GetVisibility(shadowMap, ShadowCoord);

	// Fog
	float fogCoord = abs(eye_world_pos.z/eye_world_pos.w);

	// Final color
	vec4 final_color = texture_color * s_color * (dir_light + spotlight_color) * visibility;
	FragColor = mix(final_color, fogParams.fogColor, getFogFactor(fogParams, fogCoord));
}
