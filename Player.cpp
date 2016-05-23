#include "Player.h"
#include <iostream>
#include <glm\gtx\string_cast.hpp>


Player::Player() : GameObject("Player") {
	movementSpeed = 20.0f;
	steerAngle = 0.0f;
	steerSpeed = 50.0f;
	position = vec3(10, -9.8f, 20);

	model = Model::getModel("car");

	attachedCamera = Camera::mainCamera;
	cameraRelativePosition = vec3(0, 0, 10);
	attachedCamera->attachGameObject(this, cameraRelativePosition);

	// Set spotlights
	spotLightRelativePosition = vec3(-0.65f, 0.f, -1.4f);

	distance = 0;

	spotLight = new SpotLight(	vec3(1.0f, 1.0f, 1.0f),
									position,
									normalize(-direction),
									1,
									15.0f,
									0.017f );
}


void Player::update() {
	if (enabled) {
		float deltaTime = FPSController::getDeltaTime();

		right = vec3(
			sin(glm::radians(steerAngle) - 3.14f / 2.0f),
			0,
			cos(glm::radians(steerAngle) - 3.14f / 2.0f)
			);

		direction = vec3(
			cos(0) * sin(glm::radians(steerAngle)),
			sin(0),
			cos(0) * cos(glm::radians(steerAngle))
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
		if (InputController::getInputState(true, GLFW_KEY_F) == INPUT_RELEASED) {
			spotLight->isOn = !spotLight->isOn;
		}

		// Rotate in Y Axis
		rotation.y = steerAngle;
		vec3 dir = glm::normalize(spotLight->position - position);
		spotLight->position = position;

		spotLight->direction = normalize(-direction);

		spotLight->SetUniformData();
	}
}

void Player::setEnabled(bool b) {
	GameObject::setEnabled(b);
	if (!b) {
		spotLight->isOn = false;
		spotLight->SetUniformData();
	}
}