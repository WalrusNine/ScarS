#pragma once
#include "GameObject.h"

class Sun : public GameObject
{
public:

	Sun();

	void update() override;
	void draw(bool shadow = false) override;
	glm::vec3 getDirection();

private:

	float angle;
	glm::vec3 color;
	float ambientIntensity;
	glm::vec3 direction;

};