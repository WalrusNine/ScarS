#pragma once
#include "GameObject.h"

/*
	Pressed, Released, Held
*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NUMBER_OF_KEYS 348

enum InputState {
	INPUT_NONE,
	INPUT_PRESSED,
	INPUT_HELD,
	INPUT_RELEASED
};

class InputController
{
private:
	static unsigned char keyState[NUMBER_OF_KEYS];
	static unsigned char mouseState[3];

	GLFWwindow* window;

	void updateInputState();
public:
	void update();

	InputController(GLFWwindow& window);

	static unsigned char getInputState(bool isKey, int i);

	
};
