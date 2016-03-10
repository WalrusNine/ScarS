#include "GUI.h"

void GUI::setOrtho2D(int width, int height) {
	orthoMatrix = glm::ortho(0.0f, float(width), 0.0f, float(height));
}

glm::mat4 GUI::getOrthoMatrix() {
	return orthoMatrix;
}