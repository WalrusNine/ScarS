#pragma once
#include "GameObject.h"

class Fog : public GameObject {
public:
	Fog();
	void init();
	void update() override;
	void draw(bool shadow = false) override;

private:
	float density;
	float start;
	float end;
	glm::vec4 fogColor;
	int fogEquation;	// 0 = linear, 1 = exp, 2 = exp2
};