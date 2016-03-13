#include "SkyCamera.h"

SkyCamera::SkyCamera() : Camera::Camera() {
	// There's no zoom, only movement
	// There's no attached object
	movementSpeed = 6.0f;



}

void SkyCamera::update() {
	// Use free movement controls
	float deltaTime = FPSController::getDeltaTime();

	if (InputController::getInputState(true, GLFW_KEY_LEFT) == INPUT_HELD) {
		horizontalAngle += deltaTime * speed;
	}
	if (InputController::getInputState(true, GLFW_KEY_RIGHT) == INPUT_HELD) {
		horizontalAngle -= deltaTime * speed;
	}
	if (InputController::getInputState(true, GLFW_KEY_UP) == INPUT_HELD) {
		verticalAngle += deltaTime * speed;
	}
	if (InputController::getInputState(true, GLFW_KEY_DOWN) == INPUT_HELD) {
		verticalAngle -= deltaTime * speed;
	}
	if (InputController::getInputState(true, GLFW_KEY_W) == INPUT_HELD) {
		position += direction * deltaTime * movementSpeed;
	}
	if (InputController::getInputState(true, GLFW_KEY_A) == INPUT_HELD) {
		position -= right * deltaTime * movementSpeed;
	}
	if (InputController::getInputState(true, GLFW_KEY_S) == INPUT_HELD) {
		position -= direction * deltaTime * movementSpeed;
	}
	if (InputController::getInputState(true, GLFW_KEY_D) == INPUT_HELD) {
		position += right * deltaTime * movementSpeed;
	}

	calculateMatrices();
}

void SkyCamera::calculateMatrices() {
	float rad_vert = glm::radians(verticalAngle);
	float rad_hor = glm::radians(horizontalAngle);
	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = vec3(
		cos(rad_vert) * sin(rad_hor),
		sin(rad_vert),
		cos(rad_vert) * cos(rad_hor)
		);

	right = vec3(
		sin(rad_hor - 3.14f / 2.0f),
		0,
		cos(rad_hor - 3.14f / 2.0f)
		);

	// Up vector: perpendicular to both direction and right
	up = cross(right, direction);

	projectionMatrix = perspective(fov, 4.0f / 3.0f, 0.001f, 1000.0f);
	// Look at direction
	viewMatrix = lookAt(position, position + direction, up);
}

SkyCamera* SkyCamera::createCamera() {
	SkyCamera* c = new SkyCamera();

	return c;
}