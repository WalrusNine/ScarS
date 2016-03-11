#include "SpotLight.h"


SpotLight::SpotLight() {
	color		= glm::vec3(1.0f, 1.0f, 1.0f);
	position	= glm::vec3(0.0f, 0.0f, 0.0f);
	direction	= glm::vec3(0.0f, 0.0f, -1.0f);

	isOn		= 1;
	coneAngle	= 20.0f;
	linearAtt	= 0.02f;
}

SpotLight::SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, int isOn, float coneAngle, float linearAtt) {
	this->color		= color;
	this->position	= position;
	this->direction = direction;

	this->isOn		= isOn;
	this->coneAngle = coneAngle;
	this->linearAtt = linearAtt;
}

void SpotLight::setUniformData(std::string name) {
	int loc = Shader::shader->getUniformLocation(name + "_spotlight.color");
	glUniform3fv(loc, 1, (GLfloat*)&color);

	loc = Shader::shader->getUniformLocation(name + "_spotlight.position");
	glUniform3fv(loc, 1, (GLfloat*)&position);

	loc = Shader::shader->getUniformLocation(name + "_spotlight.direction");
	glUniform3fv(loc, 1, (GLfloat*)&direction);

	loc = Shader::shader->getUniformLocation(name + "_spotlight.isOn");
	glUniform1i(loc, isOn);

	loc = Shader::shader->getUniformLocation(name + "_spotlight.coneAngle");
	glUniform1fv(loc, 1, (GLfloat*)&coneAngle);

	coneCosine = float(cos(coneAngle* (3.1415f / 180.0f)));
	loc = Shader::shader->getUniformLocation(name + "_spotlight.coneCosine");
	glUniform1fv(loc, 1, (GLfloat*)&coneCosine);

	loc = Shader::shader->getUniformLocation(name + "_spotlight.linearAtt");
	glUniform1fv(loc, 1, (GLfloat*)&linearAtt);
}