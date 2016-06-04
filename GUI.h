#pragma once
#include "GameObject.h"
#include "FreeTypeFont.h"

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class GUI : public GameObject {

public:
	GUI();
	~GUI();
	void SetOrtho2D(int width, int height);
	void SetOrtho2D();
	glm::mat4 GetOrthoMatrix();

	void draw(bool shadow = false) override;
	void update() override;

private:

	glm::mat4 orthoMatrix;
	FreeTypeFont* ftf;

	int width, height;
};