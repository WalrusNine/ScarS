#pragma once

#include <GL/glew.h>  
#include <glm/glm.hpp>
using namespace glm;

#include "GameObject.h"

class Camera : public GameObject {

public:
	Camera();
	~Camera();

	mat4 getViewMatrix();
	mat4 getProjectionMatrix();
	vec3 getPosition();
	vec3 getDirection();
	vec3 getUp();
	vec3 getViewVector();

	static Camera* createCamera();
	static Camera* mainCamera;

	void update() override;

	void attachGameObject(GameObject* go, vec3 relativePosition);

	void invertPitch();

protected:
	// Matrices for MVP
	mat4 viewMatrix;
	mat4 projectionMatrix;

	// Vectors
	vec3 direction;
	vec3 right;
	vec3 up;

	// Turn angle
	float horizontalAngle;
	float verticalAngle;
	// Field of View
	float fov;
	// Speed
	float speed;
	float zoomSpeed;

	virtual void calculateMatrices();

	GameObject* attachedGameObject;

	// Camera's zoom
	float zoom;
	float maxZoom;
	float minZoom;
};