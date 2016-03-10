#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\string_cast.hpp>
using namespace glm;

#include <iostream>

#include "InputController.h"

Camera* Camera::mainCamera;

Camera::Camera() : GameObject::GameObject() {
	position = vec3(0, 0, 40);
	
	// 180 degrees = Pi = 3.14
	horizontalAngle = 3.14f;
	verticalAngle = 0.0f;
	fov = 45.0f;
	speed = 3.0f;
	zoomSpeed = 6.0f;

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
		position = attachedGameObject->position + zoom * vec3(cos(horizontalAngle), 1, sin(horizontalAngle));
	}

	calculateMatrices();
}

void Camera::calculateMatrices() {
	vec3 up	= vec3(0, 1, 0);

	if (attachedGameObject != nullptr)
		direction = glm::normalize(attachedGameObject->position - position);
	else
		direction = glm::normalize(position);

	right = cross(direction, up);

	projectionMatrix = perspective(fov, 4.0f / 3.0f, 0.001f, 1000.0f);
	// Look at direction
	viewMatrix = lookAt(position, position + direction, up);
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