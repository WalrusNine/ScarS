#pragma once

#include "GameObject.h"
#include "FrameBuffer.h"

class Water : public GameObject {

public:
	Water();
	void update() override;
	void draw(bool shadow = false) override;

private:
	FrameBuffer reflection;
	

};