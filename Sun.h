#pragma once
#include "GameObject.h"

class Sun : public GameObject
{
public:

	Sun();

	void update() override;
	void draw() override;

private:

	float angle;
	glm::vec3 color;
	float ambientIntensity;

};