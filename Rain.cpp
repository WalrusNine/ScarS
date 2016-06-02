#include "Rain.h"
#include "Camera.h"

Rain::Rain() : GameObject("Rain") {
	enabled = false;
}

void Rain::UpdateParticles() {
	if (enabled) {
		ParticleSystem::particleSystem->SetGenPosition(glm::vec3(Camera::mainCamera->position.x, Camera::mainCamera->position.y + 50, Camera::mainCamera->position.z));
		ParticleSystem::particleSystem->SetColor({ 0, 0, 0.7f });
		ParticleSystem::particleSystem->SetLifeTime(2, 4);
		ParticleSystem::particleSystem->SetRenderedSize(0.2f);
		ParticleSystem::particleSystem->SetGravity({ 0, -10, 0 });
	}
	else {
		ParticleSystem::particleSystem->SetLifeTime(0, 0);
	}

	ParticleSystem::particleSystem->Update();
}

void Rain::update() {
	if (InputController::getInputState(true, GLFW_KEY_X) == INPUT_RELEASED) {
		enabled = !enabled;
	}
}