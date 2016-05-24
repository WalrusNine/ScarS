#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

//Include GLEW  
#include <GL/glew.h>  

//Include GLFW  
#include <GLFW/glfw3.h>

//std::vector<Shader*> Shader::shaders;
Shader* Shader::shader;
std::vector<std::pair<int, std::string>> Shader::programs;

Shader::Shader() {
	std::string dir = "data\\shaders\\";

	shaders.push_back({
		new ShaderInfo(dir + "shader.vert", GL_VERTEX_SHADER, -1),
		new ShaderInfo(dir + "shader.frag", GL_FRAGMENT_SHADER, -1)
	});
	
	shaders.push_back({
		new ShaderInfo(dir + "font_shader.vert", GL_VERTEX_SHADER, -1),
		new ShaderInfo(dir + "font_shader.frag", GL_FRAGMENT_SHADER, -1)
	});

	shader = this;
}

Shader::~Shader() {
	release();
}

void Shader::init() {
	std::vector<std::string>* sources = readSources();

	int handle;
	int nShaders = (int)shaders.size();
	int sourceCounter = 0;
	std::vector<std::string> programNames;
	programNames.push_back("main");
	programNames.push_back("font");

	// Foreach program
	for (int i = 0; i < nShaders; ++i) {

		// Foreach ShaderInfo
		for (unsigned int j = 0; j < shaders[i].size(); ++j) {
			// Set it's handle (compile)
			shaders[i][j]->handle = ((*sources)[sourceCounter].length() > 0) ? compileSource((*sources)[sourceCounter], shaders[i][j]->type) : -1;
			++sourceCounter;
		}
		// Link
		handle = linkShaders(shaders[i]);
		programs.push_back(std::pair<int, std::string>(handle, programNames[i]));

		// Foreach ShaderInfo
		for (unsigned int j = 0; j < shaders[i].size(); ++j) {
			ShaderInfo* si = shaders[i][j];
			// Delete it
			if (si->handle >= 0) {
				glDeleteShader(si->handle);
			}
		}
	}

	currentProgram = programs[0].first;

	delete sources;
}

std::vector<std::string>* Shader::readSources() {
	std::vector<std::string>* sources = new std::vector<std::string>(0);

	// Foreach program
	for (unsigned int i = 0; i < shaders.size(); ++i) {
		// Foreach ShaderInfo
		for (unsigned int j = 0; j < shaders[i].size(); ++j) {
			ShaderInfo* si = shaders[i][j];
			std::ifstream source(si->name.c_str(), std::ifstream::binary);
			(*sources).push_back(readSource(source));
		}
	}

	return sources;
}

std::string Shader::readSource(std::ifstream& stream) {
	std::string ss = "";

	std::string line;
	while (std::getline(stream, line)) {
		ss += line + '\n';
	}

	stream.close();

	return ss;
}

int Shader::compileSource(std::string source, int type) {
	int handle_aux = glCreateShader(type);

	const char* pSource = source.c_str();

	glShaderSource(handle_aux, 1, &pSource, NULL);
	glCompileShader(handle_aux);

	GLint success;
	glGetShaderiv(handle_aux, GL_COMPILE_STATUS, &success);

	if (success == GL_TRUE) {
		return handle_aux;
	}
	else {
		// printf("Error compiling shader: %s\n", pSource);
		GLint logSize = 0;
		glGetShaderiv(handle_aux, GL_INFO_LOG_LENGTH, &logSize);
		std::vector<GLchar> errorLog(logSize);
		glGetShaderInfoLog(handle_aux, logSize, &logSize, &errorLog[0]);
		for (std::vector<char>::const_iterator i = errorLog.begin(); i != errorLog.end(); ++i)
			std::cout << *i;
		return -1;
	}
}

int Shader::linkShaders(std::vector<ShaderInfo*> s) {
	int programHandle = glCreateProgram();

	for (unsigned int i = 0; i < s.size(); i++) {
		if (s[i]->handle >= 0) {
			glAttachShader(programHandle, s[i]->handle);
		}
	}

	glLinkProgram(programHandle);
	glValidateProgram(programHandle);

	GLint success;
	glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &success);

	if (success == GL_TRUE) {
		return programHandle;
	}
	else {
		printf("Couldn't link shaders.\n");
		char* errorLog = new char[1024];
		glGetProgramInfoLog(programHandle, 1024, NULL,errorLog);
		std::cout << errorLog << std::endl;
		return -1;
	}
}

void Shader::bind(std::string name) {
	for (unsigned int i = 0; i < programs.size(); ++i) {
		if (programs[i].second == name) {
			glUseProgram(programs[i].first);
			currentProgram = programs[i].first;
		}
	}
}

void Shader::dispose(std::string name) {
	for (unsigned int i = 0; i < programs.size(); ++i) {
		if (programs[i].second == name) glDeleteProgram(programs[i].first);
	}
}

int Shader::getUniformLocation(std::string varName) {
	int location = glGetUniformLocation(currentProgram, varName.c_str());
	if (location < 0) {
		//std::cout << varName << " uniform not found." << std::endl;
		return -1;
	}
	else {
		return location;
	}
}

int Shader::getAttribLocation(std::string varName) {
	int location = glGetAttribLocation(currentProgram, varName.c_str());
	if (location < 0) {
		//std::cout << varName << " not found." << std::endl;
		return -1;
	}
	else {
		return location;
	}
}

void Shader::release() {
	// Foreach program
	for (unsigned int i = 0; i < shaders.size(); ++i) {

		// Foreach ShaderInfo
		for (unsigned int j = 0; j < shaders[i].size(); ++j) {
			ShaderInfo* si = shaders[i][j];
			if (si->handle) {
				glDetachShader(programs[i].first, si->handle);
				glDeleteShader(si->handle);
				si->handle = 0;
			}
		}

		if (programs[i].first) {
			glDeleteShader(programs[i].first);
			programs[i].first = 0;
		}
	}
}

void Shader::SetAttribute(std::string name, glm::vec2 vec) {
	GLuint loc = getAttribLocation(name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)&vec);
}

// Setting floats

void Shader::SetUniform(std::string name, float* values, int count) {
	int loc = getUniformLocation(name);
	glUniform1fv(loc, count, values);
}

void Shader::SetUniform(std::string name, const float value) {
	int loc = getUniformLocation(name);
	glUniform1fv(loc, 1, &value);
}

// Setting vectors

void Shader::SetUniform(std::string name, glm::vec2* vectors, int count) {
	int loc = getUniformLocation(name);
	glUniform2fv(loc, count, (GLfloat*)vectors);
}

void Shader::SetUniform(std::string name, const glm::vec2 vector) {
	int loc = getUniformLocation(name);
	glUniform2fv(loc, 1, (GLfloat*)&vector);
}

void Shader::SetUniform(std::string name, glm::vec3* vectors, int count) {
	int loc = getUniformLocation(name);
	glUniform3fv(loc, count, (GLfloat*)vectors);
}

void Shader::SetUniform(std::string name, const glm::vec3 vector) {
	int loc = getUniformLocation(name);
	glUniform3fv(loc, 1, (GLfloat*)&vector);
}

void Shader::SetUniform(std::string name, glm::vec4* vectors, int count) {
	int loc = getUniformLocation(name);
	glUniform4fv(loc, count, (GLfloat*)vectors);
}

void Shader::SetUniform(std::string name, const glm::vec4 vector) {
	int loc = getUniformLocation(name);
	glUniform4fv(loc, 1, (GLfloat*)&vector);
}

// Setting 3x3 matrices

void Shader::SetUniform(std::string name, glm::mat3* matrices, int count) {
	int loc = getUniformLocation(name);
	glUniformMatrix3fv(loc, count, GL_FALSE, (GLfloat*)matrices);
}

void Shader::SetUniform(std::string name, const glm::mat3 matrix) {
	int loc = getUniformLocation(name);
	glUniformMatrix3fv(loc, 1, GL_FALSE, (GLfloat*)&matrix);
}

// Setting 4x4 matrices

void Shader::SetUniform(std::string name, glm::mat4* matrices, int count) {
	int loc = getUniformLocation(name);
	glUniformMatrix4fv(loc, count, GL_FALSE, (GLfloat*)matrices);
}

void Shader::SetUniform(std::string name, glm::mat4 matrix) {
	GLuint loc = getUniformLocation(name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)&matrix);
}

// Setting integers

void Shader::SetUniform(std::string name, int* values, int count) {
	int loc = getUniformLocation(name);
	glUniform1iv(loc, count, values);
}

void Shader::SetUniform(std::string name, int value) {
	GLuint loc = getUniformLocation(name);
	glUniform1i(loc, value);
}

ShaderInfo::ShaderInfo(std::string n, int t, int h) {
	this->name = n;
	this->type = t;
	this->handle = h;
}
