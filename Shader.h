#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <string>
#include <vector>

#define NUMBER_OF_SHADER_TYPES 5

class Shader {

public:
	Shader(std::string vertex, std::string fragment);
	~Shader();

	void init();
	void bind();
	void dispose();
	int getUniformLocation(std::string varName);
	int getAttribLocation(std::string varName);

	void setUniform(std::string name, glm::mat4 matrix);
	void setUniform(std::string name, int value);
	void setAttribute(std::string name, glm::vec2 vec);

	static Shader* shader;

private:
	std::string* shadersFilenames[NUMBER_OF_SHADER_TYPES];
	int shaderTypes[NUMBER_OF_SHADER_TYPES];
	int shadersHandles[NUMBER_OF_SHADER_TYPES];
	int handle;
	int programHandle;

	std::vector<std::string>* readSources();
	std::string readSource(std::ifstream& source);
	int compileSource(std::string source, int type);
	int linkShaders(int shadersHandles[]);
	
	void release();
};