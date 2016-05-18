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
	GameObject(std::string name);

	// Basic object
	virtual void update();
	virtual void draw(bool shadow = false);

	void setTransparent(bool b);
	void setEnabled(bool b);

	bool isTransparent();
	bool isEnabled();

	// Transform
	vec3 position;
	vec3 rotation;
	vec3 scale;

	vec3 rotationAxis;

	// Model
	Model* model;

	static std::vector<GameObject*> gameObjects;
	static GameObject* sun;
	static GameObject* skyBox;
	static GameObject* GetGameObjectWithName(std::string name);

private:
	// Model Matrix
	mat4 mMatrix;

	// Normal Matrix
	mat4 nMatrix;

	bool enabled;
	bool transparent;

	std::string name;

};