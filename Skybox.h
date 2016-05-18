#pragma once
#include "GameObject.h"

class Skybox : public GameObject {
public:
	Skybox();
	~Skybox();

	void update() override;
	void draw(bool shadow = false) override;

private:


};