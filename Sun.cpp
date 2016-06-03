#include "Sun.h"

Sun::Sun() : GameObject("Sun") {
	angle = 45.0f;
	float rad_angle = glm::radians(angle);

	float sine = sin(rad_angle);
	position = vec3(cos(rad_angle) * 70, sin(rad_angle) * 70, 0.0);

	ambientIntensity = 0.85f;
	color = vec3(1.0f);
}

void Sun::update() {
	float sine = 0.f;

	if (InputController::getInputState(true, GLFW_KEY_1) == INPUT_HELD) {
		angle -= (45.0f) * FPSController::getDeltaTime();
	}
	if (InputController::getInputState(true, GLFW_KEY_2) == INPUT_HELD) {
		angle += (45.0f) * FPSController::getDeltaTime();
	}

	if (angle > 90) angle = 90;
	else if (angle < -90) angle = -90;

	float rad_angle = glm::radians(angle);
	sine = sin(rad_angle);
	position = vec3(cos(rad_angle) * 70, sin(rad_angle) * 70, 0.0f);
	direction = vec3(-sin(rad_angle), -cos(rad_angle), 0);

}

void Sun::draw(bool shadow) {
	if (!shadow) {
		// Set light color
		int loc = 0;
		loc = Shader::shader->getUniformLocation("sunlight.color");
		glUniform3fv(loc, 1, &color[0]);

		// Set Ambient Intensity
		loc = Shader::shader->getUniformLocation("sunlight.ambient_intensity");
		glUniform1fv(loc, 1, &ambientIntensity);

		// Set direction
		loc = Shader::shader->getUniformLocation("sunlight.direction");
		//vec3 dir = -glm::normalize(position);
		glUniform3fv(loc, 1, &direction[0]);
	}
}

glm::vec3 Sun::getDirection() {
	return direction;
}
