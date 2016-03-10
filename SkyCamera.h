#pragma once
#include "Camera.h"

class SkyCamera : public Camera
{
public:

	SkyCamera();

	void update() override;
	void calculateMatrices() override;

	static SkyCamera* createCamera();

private:

	float movementSpeed;
};