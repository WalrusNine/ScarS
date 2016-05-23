#pragma once

#include "Shader.h"

#define NUM_PARTICLE_ATTRIBUTES 6
#define MAX_PARTICLES_ON_SCENE 100000

#define PARTICLE_TYPE_GENERATOR 0
#define PARTICLE_TYPE_NORMAL 1

/*****************************************************

Class:		CParticle

Purpose:	Encapsulates particle and its properties.

*****************************************************/

class CParticle
{
public:
	glm::vec3 vPosition;
	glm::vec3 vVelocity;
	glm::vec3 vColor;
	float fLifeTime;
	float fSize;
	int iType;
};

/**********************************************************************

Class:		CParticleSystemTransformFeedback

Purpose:	Particle system class that uses transform feedback feature.

***********************************************************************/

class CParticleSystemTransformFeedback
{
public:
	bool InitalizeParticleSystem();

	void RenderParticles();
	void UpdateParticles(float fTimePassed);

	void SetGeneratorProperties(glm::vec3 a_vGenPosition, glm::vec3 a_vGenVelocityMin, glm::vec3 a_vGenVelocityMax, glm::vec3 a_vGenGravityVector, glm::vec3 a_vGenColor, float a_fGenLifeMin, float a_fGenLifeMax, float a_fGenSize, float fEvery, int a_iNumToGenerate);

	void ClearAllParticles();
	bool ReleaseParticleSystem();

	int GetNumParticles();

	void SetMatrices(glm::mat4* a_matProjection, glm::vec3 vEye, glm::vec3 vView, glm::vec3 vUpVector);

	CParticleSystemTransformFeedback();

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

	Shader shVertexRender, shGeomRender, shFragRender;
	Shader shVertexUpdate, shGeomUpdate, shFragUpdate;
	/*ShaderProgram spRenderParticles;
	ShaderProgram spUpdateParticles;*/
};