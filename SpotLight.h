#pragma once
#include <GL\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

class SpotLight
{
public:

	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 direction;

	int isOn;
	float coneAngle;
	float linearAtt;

	void SetUniformData();

	SpotLight();
	SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, int isOn, float coneAngle, float linearAtt);

private:

	float coneCosine;


};