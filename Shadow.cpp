#include "Shadow.h"
#include "GameObject.h"
#include "Sun.h"
#include "Shader.h"
#include "Screen.h"

glm::vec3 vShadowMapQuad[] =
{
	glm::vec3(0.75f, 1.0f, 0.0f),
	glm::vec3(0.75f, 0.5f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(1.0f, 0.5f, 0.0f)
};

glm::vec2 vShadowMapQuadTC[] =
{
	glm::vec2(0.0f, 1.0f),
	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(1.0f, 0.0f)
};

Shadow::Shadow() {
	textureSize = 1024;
	vbo.CreateVBO({});
	for (int i = 0; i < 4; ++i) {
		vbo.AddData(&vShadowMapQuad[i], sizeof(glm::vec3));
		vbo.AddData(&vShadowMapQuadTC[i], sizeof(glm::vec2));
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	vbo.BindVBO();
	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));


	//shadowMap.CreateFrameBufferForDepthShadow(textureSize, textureSize);
	shadowMap.CreateFramebufferWithTexture(textureSize, textureSize);
	shadowMap.AddDepthBuffer();
	shadowMap.SetFramebufferTextureFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST);
}

void Shadow::update() {
	glm::mat4 mDepthBiasMVP;
	glm::mat4 mModel;
	// We are going to render scene from the light's point of view
	shadowMap.BindFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Because we have a directional light, we just set it high enough (vLightPos) so that it sees all objects on scene
	// We also create orthographics projection matrix for the purposes of rendering shadows
	const float fRangeX = 150, fRangeY = 150, fMinZ = 0.125f, fMaxZ = 512;
	glm::mat4 projectionFromLight = glm::ortho<float>(-fRangeX, fRangeX, -fRangeY, fRangeY, fMinZ, fMaxZ);
	glm::vec3 lightPos = -(((Sun*)GameObject::sun)->getDirection()) * 256.f;
	glm::mat4 viewFromLight = glm::lookAt(lightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	Shader::shader->SetUniform("projectionMatrix", projectionFromLight);
	Shader::shader->SetUniform("viewMatrix", viewFromLight);
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);

	// Calculate depth bias matrix to calculate shadow coordinates in shader programs
	mDepthBiasMVP = biasMatrix * projectionFromLight * viewFromLight;

	Model::BindModelsVAO();
	// Draw objects
	for (int i = 0; i < (int)GameObject::gameObjects.size(); ++i) {
		GameObject::gameObjects[i]->draw(true);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, Screen::width, Screen::height);

	int texUnit = shadowMap.GetFrameBufferTexture()->GetTextureHandle();
	Shader::shader->SetUniform("depthBiasMVP", mDepthBiasMVP);
	shadowMap.BindFramebufferTexture(texUnit, false);
	Shader::shader->SetUniform("shadowMap", texUnit);
}