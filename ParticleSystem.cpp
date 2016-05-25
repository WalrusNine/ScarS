#include "ParticleSystem.h"

#include <GL\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "FPSController.h"
#include "Texture.h"

ParticleSystem::ParticleSystem() {
	initialized = false;
	curReadBuffer = 0;

	InitalizeParticleSystem();
}

/*-----------------------------------------------

Name:	InitalizeParticleSystem

Params:	none

Result:	Initializes all buffers and data on GPU
for transform feedback particle system.

/*---------------------------------------------*/

bool ParticleSystem::InitalizeParticleSystem() {
	if (initialized) return false;

	glGenTransformFeedbacks(1, &transformFeedbackBuffer);
	glGenQueries(1, &query);

	glGenBuffers(2, particleBuffer);
	glGenVertexArrays(2, vao);

	Particle partInitialization;
	partInitialization.type = PARTICLE_TYPE_GENERATOR;

	for (int i = 0; i < 2; ++i) {
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * MAX_PARTICLES_ON_SCENE, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle), &partInitialization);

		for (int i = 0; i < NUM_PARTICLE_ATTRIBUTES; ++i) {
			glEnableVertexAttribArray(i);
		}

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)0); // Position
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)12); // Velocity
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)24); // Color
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)36); // Lifetime
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)40); // Size
		glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(Particle), (const GLvoid*)44); // Type
	}
	curReadBuffer = 0;
	numParticles = 1;

	initialized = true;

	return true;
}

/*-----------------------------------------------

Name:	UpdateParticles

Params:	fTimePassed - time passed since last frame

Result:	Performs particle updating on GPU.

/*---------------------------------------------*/

float grandf(float fMin, float fAdd)
{
	float fRandom = float(rand() % (RAND_MAX + 1)) / float(RAND_MAX);
	return fMin + fAdd*fRandom;
}

void ParticleSystem::UpdateParticles(float fTimePassed) {
	if (!initialized) return;

	Shader::shader->bind("particles_update");

	glm::vec3 vUpload;
	Shader::shader->SetUniform("fTimePassed", fTimePassed);
	Shader::shader->SetUniform("vGenPosition", genPosition);
	Shader::shader->SetUniform("vGenVelocityMin", genVelocityMin);
	Shader::shader->SetUniform("vGenVelocityRange", genVelocityRange);
	Shader::shader->SetUniform("vGenColor", genColor);
	Shader::shader->SetUniform("vGenGravityVector", genGravityVector);

	Shader::shader->SetUniform("fGenLifeMin", genLifeMin);
	Shader::shader->SetUniform("fGenLifeRange", genLifeRange);

	Shader::shader->SetUniform("fGenSize", genSize);
	Shader::shader->SetUniform("iNumToGenerate", 0);

	elapsedTime += fTimePassed;

	if (elapsedTime > nextGenerationTime)
	{
		Shader::shader->SetUniform("iNumToGenerate", numToGenerate);
		elapsedTime -= nextGenerationTime;

		glm::vec3 randomSeed = glm::vec3(grandf(-10.0f, 20.0f), grandf(-10.0f, 20.0f), grandf(-10.0f, 20.0f));
		Shader::shader->SetUniform("vRandomSeed", randomSeed);
	}

	glEnable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedbackBuffer);

	glBindVertexArray(vao[curReadBuffer]);
	glEnableVertexAttribArray(1); // Re-enable velocity

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer[1 - curReadBuffer]);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, numParticles);

	glEndTransformFeedback();

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glGetQueryObjectiv(query, GL_QUERY_RESULT, &numParticles);

	curReadBuffer = 1 - curReadBuffer;

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	Shader::shader->bind("main");
}

/*-----------------------------------------------

Name:	RenderParticles

Params:	none

Result:	Performs particle rendering on GPU.

/*---------------------------------------------*/

void ParticleSystem::RenderParticles() {
	if (!initialized) return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(0);

	glDisable(GL_RASTERIZER_DISCARD);

	Shader::shader->bind("particles_render");
	Shader::shader->SetUniform("matrices.mProj", projection);
	Shader::shader->SetUniform("matrices.mView", view);
	Shader::shader->SetUniform("vQuad1", quad1);
	Shader::shader->SetUniform("vQuad2", quad2);
	Shader::shader->SetUniform("gSampler", 0);

	glBindVertexArray(vao[curReadBuffer]);
	glDisableVertexAttribArray(1); // Disable velocity, because we don't need it for rendering

	glDrawArrays(GL_POINTS, 0, numParticles);

	glDepthMask(1);
	glDisable(GL_BLEND);

	Shader::shader->bind("main");
}

/*-----------------------------------------------

Name:	SetMatrices

Params:	a_matProjection - projection matrix
vEye, vView, vUpVector - definition of view matrix

Result:	Sets the projection matrix and view matrix,
that shaders of transform feedback particle system
need.

/*---------------------------------------------*/

void ParticleSystem::SetMatrices(glm::mat4* a_matProjection, glm::vec3 vEye, glm::vec3 vView, glm::vec3 vUpVector) {
	projection = *a_matProjection;

	view = glm::lookAt(vEye, vView, vUpVector);
	vView = vView - vEye;
	vView = glm::normalize(vView);
	quad1 = glm::cross(vView, vUpVector);
	quad1 = glm::normalize(quad1);
	quad2 = glm::cross(vView, quad1);
	quad2 = glm::normalize(quad2);
}

/*-----------------------------------------------

Name:	SetGeneratorProperties

Params:	many properties of particle generation

Result:	Guess what it does :)

/*---------------------------------------------*/

void ParticleSystem::SetGeneratorProperties(glm::vec3 a_vGenPosition, glm::vec3 a_vGenVelocityMin, glm::vec3 a_vGenVelocityMax, glm::vec3 a_vGenGravityVector, glm::vec3 a_vGenColor, float a_fGenLifeMin, float a_fGenLifeMax, float a_fGenSize, float fEvery, int a_iNumToGenerate) {
	genPosition = a_vGenPosition;
	genVelocityMin = a_vGenVelocityMin;
	genVelocityRange = a_vGenVelocityMax - a_vGenVelocityMin;

	genGravityVector = a_vGenGravityVector;
	genColor = a_vGenColor;
	genSize = a_fGenSize;

	genLifeMin = a_fGenLifeMin;
	genLifeRange = a_fGenLifeMax - a_fGenLifeMin;

	nextGenerationTime = fEvery;
	elapsedTime = 0.8f;

	numToGenerate = a_iNumToGenerate;
}

/*-----------------------------------------------

Name:	GetNumParticles

Params:	none

Result:	Retrieves current number of particles.

/*---------------------------------------------*/

int ParticleSystem::GetNumParticles() {
	return numParticles;
}

void ParticleSystem::SetGenPosition(glm::vec3 pos) {
	genPosition = pos;
}

void ParticleSystem::SetColor(glm::vec3 c) {
	genColor = c;
}

void ParticleSystem::SetLifeTime(float min, float max) {
	genLifeMin = min;
	genLifeRange = max;
}

void ParticleSystem::SetRenderedSize(float s) {
	genSize = s;
}

void ParticleSystem::SetGravity(glm::vec3 v) {
	genGravityVector = v;
}

void ParticleSystem::Update() {
	// Particles
	// Bind texture
	Texture::GetTexture("particle")->Bind();
	SetMatrices(&Camera::mainCamera->getProjectionMatrix(), Camera::mainCamera->getPosition(), Camera::mainCamera->getViewVector(), Camera::mainCamera->getUp());
	UpdateParticles(FPSController::getDeltaTime());
	RenderParticles();
}