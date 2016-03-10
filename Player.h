#pragma once

#include "GameObject.h"
#include "Camera.h"

class Player : public GameObject
{
public:
	Player();
	~Player();

	void update() override;
	//void draw() override;

private:
	Camera* attachedCamera;
	vec3 cameraRelativePosition;

	float movementSpeed;
	float steerSpeed;

	float steerAngle;

	vec3 direction;
	vec3 right;

};