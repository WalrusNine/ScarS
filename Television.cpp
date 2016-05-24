#include "Television.h"

#include "Camera.h"
#include "Screen.h"

Television::Television() : GameObject("television") {

}

void Television::draw(bool shadow) {
	GameObject::draw(shadow);
}

void Television::create() {
	model = Model::getModel("panel");
	position.x = 15;
	position.z = 0;
	position.y = -5.f;
	rotation.x = 90;
	scale.x *= 10;
	scale.z *= 5;

	frameBuffer.CreateFramebufferWithTexture(1024, 720);
	frameBuffer.AddDepthBuffer();
	frameBuffer.SetFramebufferTextureFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_TRILINEAR);

	model->setTexture(frameBuffer.GetFrameBufferTexture());
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void Television::startDrawing() {
	frameBuffer.BindFramebuffer(false);
	int projectionMatrixLocation = Shader::shader->getUniformLocation("projectionMatrix");
	int viewMatrixLocation = Shader::shader->getUniformLocation("viewMatrix");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Use television's parameter to set view/projection
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &frameBuffer.CalculateProjectionMatrix(45.0f, 0.001f, 1000.0f)[0][0]);

	glm::vec3 cameraPos = position + glm::vec3(.0f, -1.0f, -5.0f);

	glm::mat4 televisionView = glm::lookAt(cameraPos, cameraPos + glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &televisionView[0][0]);
}

void Television::stopDrawing() {
	// Go back
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	glViewport(0, 0, Screen::width, Screen::height);
	Camera::mainCamera->UpdateProjView();
}



void Television::update() {
	startDrawing();

	GameObject* gui = GameObject::GetGameObjectWithName("gui");

	// Render to Television
	// Draw objects
	for (int i = 0; i < (int)GameObject::gameObjects.size(); ++i) {
		if (GameObject::gameObjects[i] != this && GameObject::gameObjects[i] != gui)
			GameObject::gameObjects[i]->draw();
	}
		
	stopDrawing();
}
