#include "Wolf.h"

Wolf::Wolf() : GameObject("Wolf") {
	position = vec3(0, 0, 10);
	rotation = vec3(0.0f);
	scale = vec3(1.0f);

	model = Model::getModel("wolf");
}

void Wolf::update() {
	if (InputController::getInputState(true, GLFW_KEY_J) == INPUT_HELD) {
		position.x -= 3 * FPSController::getDeltaTime();
	}
	if (InputController::getInputState(true, GLFW_KEY_L) == INPUT_HELD) {
		position.x += 3 * FPSController::getDeltaTime();
	}
	if (InputController::getInputState(true, GLFW_KEY_I) == INPUT_HELD) {
		position.z += 3 * FPSController::getDeltaTime();
	}
	if (InputController::getInputState(true, GLFW_KEY_K) == INPUT_HELD) {
		position.z -= 3 * FPSController::getDeltaTime();
	}
}