#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "Model.h"
#include "FPSController.h"
#include "InputController.h"

class GameObject {

public:
	GameObject();

	// Basic object
	virtual void update();
	virtual void draw();

	// Transform
	vec3 position;
	vec3 rotation;
	vec3 scale;

	vec3 rotationAxis;

	// Model
	Model* model;

private:
	// Model Matrix
	mat4 mMatrix;

	// Normal Matrix
	mat4 nMatrix;

	bool isEnabled;

};