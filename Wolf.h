#pragma once

#include "GameObject.h"

class Wolf : public GameObject
{
public:
	Wolf();
	void update() override;
};