#include "Player.h"

#include <glm\gtx\string_cast.hpp>


Player::Player() {
	GameObject::GameObject();

	movementSpeed = 20.0f;
	steerAngle = 0.0f;
	steerSpeed = 50.0f;
	position = vec3(10, -9.8f, 20);

	model = Model::getModel("car");

	attachedCamera = Camera::mainCamera;
	cameraRelativePosition = vec3(0, 0, 10);
	attachedCamera->attachGameObject(this, cameraRelativePosition);

	// Set spotlights
	rightSpotLightRelativePosition = vec3(0.65f, 0.6f, -1.4f);
	leftSpotLightRelativePosition = vec3(-0.65f, 0.6f, -1.4f);

	leftSpotLight = new SpotLight(	vec3(1.0f, 1.0f, 1.0f),
									position + leftSpotLightRelativePosition,
									normalize(direction * 75.0f),
									1,
									15.0f,
									0.017f );
	rightSpotLight = new SpotLight(	vec3(1.0f, 1.0f, 1.0f),
									position + rightSpotLightRelativePosition,
									direction,
									1,
									15.0f,
									0.017f);
}

void Player::update() {
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
		leftSpotLight->isOn = !leftSpotLight->isOn;
		rightSpotLight->isOn = !rightSpotLight->isOn;
	}
	
	// Rotate in Y Axis
	rotation.y = steerAngle;

	leftSpotLight->direction = normalize(-direction);
	leftSpotLight->position = position + leftSpotLightRelativePosition;

	rightSpotLight->direction = normalize(-direction);
	rightSpotLight->position = position + rightSpotLightRelativePosition;

	leftSpotLight->setUniformData("left");
	rightSpotLight->setUniformData("right");
}