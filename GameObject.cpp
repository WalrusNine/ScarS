#include "GameObject.h"

GameObject::GameObject() {
	position = vec3(0.0f);
	rotation = vec3(0.0f);
	scale = vec3(1.0f);

	rotationAxis = vec3(0, 0, 0);

	model = nullptr;
}

void GameObject::update() {

}

void GameObject::draw() {
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
		Shader::shader->setUniform("modelMatrix", mMatrix);
		Shader::shader->setUniform("normalMatrix", nMatrix);

		// Render model
		model->render();
	}
}

// SET ISENABLE