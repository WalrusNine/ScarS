#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class GUI {

public:
	void setOrtho2D(int width, int height);
	glm::mat4 getOrthoMatrix();

private:

	glm::mat4 orthoMatrix;

};