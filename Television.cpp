#include "Television.h"

#include "Camera.h"
/*
glm::vec3 vCinema[4] =
{
	glm::vec3(-30.0f, 40.0f, -70.0f), glm::vec3(-30.0f, 10.0f, -70.0f),
	glm::vec3(30.0f, 40.0f, -70.0f), glm::vec3(30.0f, 10.0f, -70.0f)
};

glm::vec2 vCinemaCoords[4] =
{
	glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(1, 0)
};*/

glm::vec3 vCinema[6] =
{
	// Triangle 1
	{ -1.0f, 0.0f, -1.0f },{ 1.0f, 0.0f, -1.0f },{ 1.0f, 0.f, 1.0f },
	// Triangle 2
	{ 1.0f, 0.f, 1.0f },{ -1.0f, 0.f, 1.0f },{ -1.0f, 0.f, -1.0f }
};

glm::vec2 vCinemaCoords[6] =
{
	// Triangle 1
	{ 0.0f, 1.0f },{ 1.0f, 1.0f },{ 1.0f, 0.0f },
	// Triangle 2
	{ 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f },
};

void Television::draw() {
	//glBindVertexArray(vao);
	//frameBuffer.BindFramebufferTexture(0, true);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	model->render();
}

void Television::create() {
	model = Model::getModel("panel");
	/*vbo.createVBO({});
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	vbo.bindVBO();
	for (int i = 0; i < 6; ++i) {
		vbo.addData(&vCinema[i], sizeof(glm::vec3));
		vbo.addData(&vCinemaCoords[i], sizeof(glm::vec2));
		glm::vec3 vNormal(0, 0, 1);
		vbo.addData(&vNormal, sizeof(glm::vec3));
		glm::vec4 color(1, 1, 1, 1);
		vbo.addData(&color, sizeof(glm::vec4));
	}
	vbo.uploadDataToGPU();
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4), 0);
	// Texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	
	Model::BindModelsVAO();*/

	frameBuffer.CreateFramebufferWithTexture(512, 256);
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

	glm::mat4 televisionView = Camera::mainCamera->getViewMatrix();//glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &televisionView[0][0]);
}

void Television::stopDrawing() {
	// Go back
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	glViewport(0, 0, 1280, 720);
}



void Television::update() {

}
