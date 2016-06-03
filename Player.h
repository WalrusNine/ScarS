#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "SpotLight.h"
#include "ParticleSystem.h"

class Player : public GameObject
{
public:
	Player();
	~Player();

	void update() override;
	void draw(bool shadow = false) override;
	void UpdateParticles() override;
	void setEnabled(bool b) override;

	ParticleSystem* particles;

private:
	Camera* attachedCamera;
	vec3 cameraRelativePosition;

	float movementSpeed;
	float steerSpeed;

	float steerAngle;

	vec3 direction;
	vec3 right;

	SpotLight* spotLight;
	vec3 spotLightRelativePosition;

	float distance;

	bool smokeEnabled;

	GameObject* carShadow;
};