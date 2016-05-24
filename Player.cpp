#include "Player.h"
#include <iostream>
#include <glm\gtx\string_cast.hpp>


Player::Player() : GameObject("player") {
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

	particles = new ParticleSystem();
	particles->SetGeneratorProperties(
		position + glm::vec3(-1.f, 0.f, -1.f), // Where the particles are generated
		glm::vec3(-5, 0, -5), // Minimal velocity
		glm::vec3(5, 20, 5), // Maximal velocity
		glm::vec3(0, -5, 0), // Gravity force applied to particles
		glm::vec3(0.01f, 0.01f, 0.01f), // Color
		1.5f, // Minimum lifetime in seconds
		3.0f, // Maximum lifetime in seconds
		0.75f, // Rendered size
		0.02f, // Spawn every 0.05 seconds
		30); // And spawn 30 particles
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

		// Particles
		particles->SetGenPosition(position + glm::vec3(0, 0, 0));
	}
}

void Player::setEnabled(bool b) {
	GameObject::setEnabled(b);
	if (!b) {
		spotLight->isOn = false;
		spotLight->SetUniformData();
	}
}