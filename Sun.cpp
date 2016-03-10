#include "Sun.h"

Sun::Sun() : GameObject::GameObject() {
	angle = 45.0f;

	float sine = sin(angle*3.1415 / 180.0);
	position = vec3(cos(angle*3.1415 / 180.0) * 70, sin(angle*3.1415 / 180.0) * 70, 0.0);

	ambientIntensity = 0.8f;
}

void Sun::update() {
	int loc = 0;
	loc = Shader::shader->getUniformLocation("sunlight.color");
	glUniform3fv(loc, 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
	loc = Shader::shader->getUniformLocation("sunlight.ambient_intensity");
	float ambientIntensity = 0.25f;
	glUniform1fv(loc, 1, &ambientIntensity);
	loc = Shader::shader->getUniformLocation("sunlight.direction");
	vec3 dir = -glm::normalize(position);
	glUniform3fv(loc, 1, &dir[0]);
}

void Sun::draw() {

}