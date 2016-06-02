#include "GameObject.h"

std::vector<GameObject*> GameObject::gameObjects;
GameObject* GameObject::sun;
GameObject* GameObject::skyBox;

GameObject::GameObject(std::string name) {
	position = vec3(0.0f);
	rotation = vec3(0.0f);
	scale = vec3(1.0f);

	rotationAxis = vec3(0, 0, 0);

	model = nullptr;

	enabled = true;
	transparent = false;

	this->name = name;
}

void GameObject::update() {

}

void GameObject::draw(bool shadow) {
	if (enabled) {
		if (model != nullptr) {
			// Update Model Matrix
			// Third - Translate
			mMatrix = translate(mat4(1.0f), position);
			// Second - Rotate (translate to pivot and rotate)
			mMatrix = translate(mMatrix, rotationAxis);
			mMatrix = rotate(mMatrix, glm::radians(rotation.x), vec3(1, 0, 0));
			mMatrix = rotate(mMatrix, glm::radians(rotation.y), vec3(0, 1, 0));
			mMatrix = rotate(mMatrix, glm::radians(rotation.z), vec3(0, 0, 1));
			mMatrix = translate(mMatrix, -rotationAxis);
			// First - Scale
			mMatrix = glm::scale(mMatrix, scale);

			// Update Normal Matrix
			nMatrix = glm::transpose(glm::inverse(mMatrix));

			// Set uniforms
			Shader::shader->SetUniform("modelMatrix", mMatrix);
			Shader::shader->SetUniform("normalMatrix", nMatrix);

			// Transparency
			if (transparent) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDepthMask(0); // Disable writing to depth buffer
			}

			// Render model
			model->render();

			if (transparent) {
				glDisable(GL_BLEND);
				glDepthMask(1); // Re-enable writing to depth buffer
			}
		}
	}
}

void GameObject::UpdateParticles() {}

void GameObject::setTransparent(bool b) {
	transparent = b;
}

void GameObject::setEnabled(bool b) {
	enabled = b;
}

bool GameObject::isTransparent() {
	return transparent;
}

bool GameObject::isEnabled() {
	return enabled;
}

GameObject * GameObject::GetGameObjectWithName(std::string name) {
	int len = gameObjects.size();
	for (int i = 0; i < len; ++i) {
		if (gameObjects[i]->name == name) return gameObjects[i];
	}

	return nullptr;
}
