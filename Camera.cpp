#include "Camera.h"
#include "SkyCamera.h"
#include "Player.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\string_cast.hpp>
using namespace glm;

#include <iostream>

#include "InputController.h"

Camera* Camera::mainCamera;

Camera::Camera() : GameObject::GameObject("Camera") {
	position = vec3(0, 0, 1);

	isSkyCamera = false;
	
	// 180 degrees = Pi = 3.14
	horizontalAngle = 3.14f;
	verticalAngle = 0.0f;
	fov = 45.0f;
	speed = 100.0f;
	zoomSpeed = 6.0f;
	movementSpeed = 18.0f;

	attachedGameObject = nullptr;
	zoom = 0;
	maxZoom = 1.0f;
	minZoom = 1.0f;

	calculateMatrices();
}

mat4 Camera::getProjectionMatrix() {
	return projectionMatrix;
}

mat4 Camera::getViewMatrix() {
	return viewMatrix;
}

vec3 Camera::getPosition() {
	return position;
}

void Camera::update() {
	if (InputController::getInputState(true, GLFW_KEY_Z) == INPUT_RELEASED) {
		isSkyCamera = !isSkyCamera;
		Player* player = (Player*)GameObject::GetGameObjectWithName("player");
		player->setEnabled(!isSkyCamera);
	}

	if (isSkyCamera) {
		UpdateSkyCamera();
	}
	else {
		UpdateCarCamera();
	}

	calculateMatrices();

	UpdateProjView();
}

void Camera::UpdateProjView() {
	Shader::shader->SetUniform("projectionMatrix", getProjectionMatrix());
	Shader::shader->SetUniform("viewMatrix", getViewMatrix());
}

void Camera::calculateMatrices() {
	if (isSkyCamera) {
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
	else {
		up = vec3(0, 1, 0);

		if (attachedGameObject != nullptr)
			direction = glm::normalize(attachedGameObject->position - position);
		else
			direction = glm::normalize(position);

		right = cross(direction, up);

		projectionMatrix = perspective(fov, 4.0f / 3.0f, 0.001f, 1000.0f);
		// Look at direction
		viewMatrix = lookAt(position, position + direction, up);
	}
	
}

void Camera::UpdateSkyCamera() {
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
}

void Camera::UpdateCarCamera() {
	float deltaTime = FPSController::getDeltaTime();
	if (InputController::getInputState(true, GLFW_KEY_LEFT) == INPUT_HELD) {
		horizontalAngle -= deltaTime * speed;
	}
	if (InputController::getInputState(true, GLFW_KEY_RIGHT) == INPUT_HELD) {
		horizontalAngle += deltaTime * speed;
	}
	if (InputController::getInputState(true, GLFW_KEY_UP) == INPUT_HELD) {
		// Zoom in
		zoom -= deltaTime * zoomSpeed;
		if (zoom < maxZoom)
			zoom = maxZoom;
	}
	if (InputController::getInputState(true, GLFW_KEY_DOWN) == INPUT_HELD) {
		// Zoom out
		zoom += deltaTime * zoomSpeed;
		if (zoom > minZoom)
			zoom = minZoom;
	}

	if (attachedGameObject != nullptr) {
		// Put camera at right distance from attached object
		position = attachedGameObject->position + zoom * direction;
		// Get distance between camera and attached object
		zoom = glm::distance(attachedGameObject->position, position);
		// Move camera around the car
		position = attachedGameObject->position + zoom * vec3(cos(glm::radians(horizontalAngle)), 1, sin(glm::radians(horizontalAngle)));
	}
}

Camera* Camera::createCamera() {
	Camera* c = new Camera();

	return c;
}

void Camera::attachGameObject(GameObject* go, vec3 relativePosition) {
	attachedGameObject = go;
	position = attachedGameObject->position + relativePosition;
	zoom = glm::distance(attachedGameObject->position, position);
	// TODO: Get max/min values through attached object
	minZoom = 15.0f;
	maxZoom = 5.0f;
}

void Camera::invertPitch() {
	// If vertical angle is 30 has to be -30
	verticalAngle = -verticalAngle;
}

vec3 Camera::getDirection() {
	return direction;
}

vec3 Camera::getUp() {
	return up;
}

vec3 Camera::getViewVector() {
	return position + direction;
}
