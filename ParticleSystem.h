#pragma once

#include "Shader.h"

#define NUM_PARTICLE_ATTRIBUTES 6
#define MAX_PARTICLES_ON_SCENE 100000

#define PARTICLE_TYPE_GENERATOR 0
#define PARTICLE_TYPE_NORMAL 1

class Particle {
public:

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 color;

	float lifeTime;
	float size;
	int type;
};

class ParticleSystem {
public:

	static ParticleSystem* particleSystem;

	bool InitalizeParticleSystem();

	void RenderParticles();
	void UpdateParticles(float fTimePassed);

	void SetGeneratorProperties(glm::vec3 a_vGenPosition, glm::vec3 a_vGenVelocityMin, glm::vec3 a_vGenVelocityMax, glm::vec3 a_vGenGravityVector, glm::vec3 a_vGenColor, float a_fGenLifeMin, float a_fGenLifeMax, float a_fGenSize, float fEvery, int a_iNumToGenerate);
	void SetGenPosition(glm::vec3);
	void SetColor(glm::vec3);
	void SetLifeTime(float min, float max);
	void SetRenderedSize(float s);
	void SetGravity(glm::vec3 v);

	void Update();

	int GetNumParticles();

	void SetMatrices(glm::mat4* a_matProjection, glm::vec3 vEye, glm::vec3 vView, glm::vec3 vUpVector);

	ParticleSystem();

private:
	bool initialized;

	unsigned int transformFeedbackBuffer;

	unsigned int particleBuffer[2];
	unsigned int vao[2];

	unsigned int query;
	unsigned int texture;

	int curReadBuffer;
	int numParticles;

	glm::mat4 projection, view;
	glm::vec3 quad1, quad2;

	float elapsedTime;
	float nextGenerationTime;

	glm::vec3 genPosition;
	glm::vec3 genVelocityMin, genVelocityRange;
	glm::vec3 genGravityVector;
	glm::vec3 genColor;

	float genLifeMin, genLifeRange;
	float genSize;

	int numToGenerate;
};