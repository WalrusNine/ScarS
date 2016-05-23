#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <string>
#include <vector>

#define NUMBER_OF_SHADER_TYPES 2

class Shader {

public:
	Shader();
	~Shader();

	void init();
	void bind();
	void dispose();
	int getUniformLocation(std::string varName);
	int getAttribLocation(std::string varName);

	void SetUniform(std::string name, glm::vec2 * vectors, int count);
	void SetUniform(std::string name, const glm::vec2 vector);
	void SetUniform(std::string name, glm::vec3 * vectors, int count);
	void SetUniform(std::string name, const glm::vec3 vector);
	void SetUniform(std::string name, glm::vec4 * vectors, int count);
	void SetUniform(std::string name, const glm::vec4 vector);
	void SetUniform(std::string name, glm::mat3 * matrices, int count);
	void SetUniform(std::string name, const glm::mat3 matrix);
	void SetUniform(std::string name, glm::mat4 * matrices, int count);
	void SetUniform(std::string name, glm::mat4 matrix);
	void SetUniform(std::string name, int * values, int count);
	void SetUniform(std::string name, int value);
	void SetUniform(std::string name, float * values, int count);
	void SetUniform(std::string name, const float value);

	void SetAttribute(std::string name, glm::vec2 vec);

	static Shader* shader;
	//static Shader* GetShader(std::string name);

private:
	std::string* shadersFilenames[NUMBER_OF_SHADER_TYPES];
	int shaderTypes[NUMBER_OF_SHADER_TYPES];
	int shadersHandles[NUMBER_OF_SHADER_TYPES];
	int handle;
	int programHandle;

	std::vector<std::string>* readSources();
	std::string readSource(std::ifstream& source);
	int compileSource(std::string source, int type);
	int linkShaders(std::vector<int> shadersHandles);
	
	void release();

	static std::vector<std::pair<int, std::string>> programs;

	//static std::vector<Shader*> shaders;
};