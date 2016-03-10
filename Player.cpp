#include "Player.h"


Player::Player() {
	GameObject::GameObject();

	movementSpeed = 20.0f;
	steerAngle = 0.0f;
	steerSpeed = 2.0f;
	position = vec3(10, -9.8f, 20);

	model = Model::getModel("car");

	attachedCamera = Camera::mainCamera;
	cameraRelativePosition = vec3(0, 0, 10);
	attachedCamera->attachGameObject(this, cameraRelativePosition);
}

void Player::update() {
	float deltaTime = FPSController::getDeltaTime();

	right = vec3(
		sin(steerAngle - 3.14f / 2.0f),
		0,
		cos(steerAngle - 3.14f / 2.0f)
		);

	direction = vec3(
		cos(0) * sin(steerAngle),
		sin(0),
		cos(0) * cos(steerAngle)
		);
	
	if (InputController::getInputState(true, GLFW_KEY_W) == INPUT_HELD) {
		position -= direction * deltaTime * movementSpeed;
	}
	if (InputController::getInputState(true, GLFW_KEY_A) == INPUT_HELD) {
		steerAngle += deltaTime * steerSpeed;
	}
	if (InputController::getInputState(true, GLFW_KEY_S) == INPUT_HELD) {
		position += direction * deltaTime * movementSpeed;
	}
	if (InputController::getInputState(true, GLFW_KEY_D) == INPUT_HELD) {
		steerAngle -= deltaTime * steerSpeed;
	}
	
	// Rotate in Y Axis
	rotation.y = steerAngle;
}