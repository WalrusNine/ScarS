#include "Water.h"
#include "Camera.h"

Water::Water() : GameObject("water") {
	model = Model::getModel("water");
	scale.x *= 500;
	scale.z *= 500;
	position.y = -15;

	// Create FrameBuffers
	//reflection.CreateFramebufferWithTexture(320, 180);
	reflection.CreateFramebufferWithTexture(1280, 720);
	reflection.AddDepthBuffer();
	reflection.SetFramebufferTextureFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_TRILINEAR);

	model->setTexture(reflection.GetFrameBufferTexture());
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);

	Shader::shader->SetUniform("reflectionTexture", (int)reflection.GetFrameBufferTexture()->GetTextureHandle());


}

void Water::update() {
	// Camera has to go 2 * (camera.y - water.height) down
	float d = 2 * (Camera::mainCamera->position.y - position.y);
	Camera::mainCamera->position.y -= d;
	/*glm::vec3 cameraPos = Camera::mainCamera->position;
	cameraPos.y -= */
	// Then invert pitch
	Camera::mainCamera->invertPitch();
	// Render scene to reflection
	reflection.BindFramebuffer(false);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < (int)gameObjects.size(); ++i) {
		if (gameObjects[i] != this)
			gameObjects[i]->draw(false);
	}
	// Go back
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	glViewport(0, 0, 1280, 720);
	Camera::mainCamera->position.y += d;
	Camera::mainCamera->invertPitch();
}

void Water::draw(bool shadow) {
	GameObject::draw(shadow);
}