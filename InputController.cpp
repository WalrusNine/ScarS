#include "InputController.h"

//Include GLFW  
#include <GLFW/glfw3.h>

#include <stdio.h>

unsigned char InputController::keyState[NUMBER_OF_KEYS];
unsigned char InputController::mouseState[3];

InputController::InputController(GLFWwindow& window) {
	//Initialize input state
	for (int i = 0; i < NUMBER_OF_KEYS; ++i) {
		//Pressed
		keyState[i] = INPUT_NONE;
		//Released
		keyState[i] = INPUT_NONE;
		//Held
		keyState[i] = INPUT_NONE;
	}

	mouseState[0] = INPUT_NONE;
	mouseState[1] = INPUT_NONE;
	mouseState[2] = INPUT_NONE;

	this->window = &window;
}

void InputController::update() {
	//First update keyboard and mouse states
	updateInputState();
}

void InputController::updateInputState() {
	int state;
	for (int i = 0; i < NUMBER_OF_KEYS; ++i) {
		state = glfwGetKey(window, i);
		switch (keyState[i]) {
			case INPUT_NONE:
				if (state == GLFW_PRESS) {
					keyState[i] = INPUT_PRESSED;
				}
				break;
			case INPUT_PRESSED:
				if (state == GLFW_PRESS){
					keyState[i] = INPUT_HELD;
				}
				else {
					keyState[i] = INPUT_RELEASED;
				}
				break;
			case INPUT_HELD:
				if (state != GLFW_PRESS) {
					keyState[i] = INPUT_RELEASED;
				}
				break;
			case INPUT_RELEASED:
				if (state == GLFW_PRESS) {
					keyState[i] = INPUT_PRESSED;
				}
				else {
					keyState[i] = INPUT_NONE;
				}
				break;
		}
	}
}

unsigned char InputController::getInputState(bool isKey, int i) {
	if (isKey) return keyState[i];
	else return mouseState[i];
}