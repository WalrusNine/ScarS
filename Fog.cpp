#include "Fog.h"

Fog::Fog() : GameObject("Fog") {

}

void Fog::init() {
	// Set params
	density = 0.04f;
	start = 10.0f;
	end = 75.0f;
	fogColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	fogEquation = 1; // 0 = linear, 1 = exp, 2 = exp2
}

void Fog::update() {
	if (InputController::getInputState(true, GLFW_KEY_G) == INPUT_RELEASED) {
		enabled = !enabled;
	}

	if (enabled) density = 0.04f;
	else density = 0.f;
}

void Fog::draw(bool shadow) {
	Shader::shader->SetUniform("fogParams.equation", fogEquation);
	Shader::shader->SetUniform("fogParams.fogColor", fogColor);

	if (fogEquation == 0) {
		Shader::shader->SetUniform("fogParams.start", start);
		Shader::shader->SetUniform("fogParams.end", end);
	}
	else {
		Shader::shader->SetUniform("fogParams.density", density);
	}
}
