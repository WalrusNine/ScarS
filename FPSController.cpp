#include "FPSController.h"

//Include GLFW  
#include <GLFW/glfw3.h>

float FPSController::deltaTime = 0.0f;
/*
FPSController& FPSController::getInstance() {
	static FPSController instance;
	return instance;
}
*/

FPSController::FPSController() {
	lastTime	= glfwGetTime();
	deltaTime	= 0;
}

void FPSController::update() {
	double currentTime = glfwGetTime();

	deltaTime = float(currentTime - lastTime);

	lastTime = currentTime;
}

float FPSController::getDeltaTime() {
	return deltaTime;
}