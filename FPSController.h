#pragma once

class FPSController {

private:

	double lastTime;
	static float deltaTime;

public:
	//static FPSController& getInstance();
	FPSController();
	void update();
	static float getDeltaTime();
};