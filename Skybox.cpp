#include <GL\glew.h>
#include "Skybox.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Skybox::Skybox()
{
	position = Camera::mainCamera->getPosition();
	rotation = vec3(0.0f);
	scale = vec3(1.0f);

	model = nullptr;
}

Skybox::~Skybox()
{

}

void Skybox::update() {
	position = Camera::mainCamera->getPosition();
}

void Skybox::draw() {
	if (model != nullptr) {
		// Depth mask = 0 => draw before anything,
		// so that it always stay away from anything in the scene
		glDepthMask(0);
		GameObject::draw();
		glDepthMask(1);
	}
}