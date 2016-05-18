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

Shader* Shader::shader;

Shader::Shader(std::string vertex, std::string fragment) {
	handle = -1;

	this->shadersFilenames[0] = new std::string(vertex);
	this->shadersFilenames[1] = new std::string(fragment);
	this->shadersFilenames[2] = new std::string("shadowMapper.frag");
	this->shadersFilenames[3] = new std::string("shadowMapper.vert");

	this->shaderTypes[0] = GL_VERTEX_SHADER;
	this->shaderTypes[1] = GL_FRAGMENT_SHADER;
	this->shaderTypes[2] = GL_FRAGMENT_SHADER;
	this->shaderTypes[3] = GL_VERTEX_SHADER;

	shader = this;
}

Shader::~Shader() {
	release();

	for (int i = 0; i < NUMBER_OF_SHADER_TYPES; ++i) {
		delete shadersFilenames[i];
	}
}

void Shader::init() {
	std::vector<std::string>* sources = readSources();

	for (int i = 0; i < NUMBER_OF_SHADER_TYPES; ++i) {
		shadersHandles[i] = ((*sources)[i].length() > 0) ? compileSource((*sources)[i], shaderTypes[i]) : -1;
		// printf("Shader[%d]: %d\n", i, shadersHandles[i]);
	}

	handle = linkShaders(shadersHandles);

	for (int i = 0; i < NUMBER_OF_SHADER_TYPES; ++i) {
		if (shadersHandles[i] >= 0) {
			glDeleteShader(shadersHandles[i]);
		}
	}

	delete sources;
}

std::vector<std::string>* Shader::readSources() {
	std::vector<std::string>* sources = new std::vector<std::string>(5);

	for (int i = 0; i < NUMBER_OF_SHADER_TYPES; ++i) {
		if (shadersFilenames[i] != NULL) {
			std::ifstream source(shadersFilenames[i]->c_str(), std::ifstream::binary);
			(*sources)[i] = readSource(source);
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
	}
	/*ERROR treatment
	else {
		glGetShaderiv(handle_aux, GL_INFO_LOG_LENGTH, buffer);

		ByteBuffer byteBuffer = Buffers.newDirectByteBuffer(buffer
			.get(0));
		glGetShaderInfoLog(handle_aux, byteBuffer.capacity(), buffer,
			byteBuffer);

		
		printf("\nshader compile error: ");
		for (int i = 0; i < buffer.get(0); i++) {
			System.err.print((char)byteBuffer.get(i));
		}
		

		return -1;
	}
	*/
}

int Shader::linkShaders(int shadersHandles[]) {
	int programHandle = glCreateProgram();

	for (int i = 0; i < NUMBER_OF_SHADER_TYPES; i++) {
		if (shadersHandles[i] >= 0) {
			glAttachShader(programHandle, shadersHandles[i]);
		}
	}

	glLinkProgram(programHandle);
	glValidateProgram(programHandle);

	GLint success;
	glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &success);

	if (success == GL_TRUE) {
		return programHandle;
	}
	else printf("Couldn't link shaders.\n");
	/* ERROR handling
	else {

		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, buffer);

		ByteBuffer byteBuffer = Buffers.newDirectByteBuffer(buffer
			.get(0));
		glGetProgramInfoLog(programHandle, byteBuffer.capacity(), buffer,
			byteBuffer);

		System.err.println("\nshader link error: ");
		for (int i = 0; i < buffer.get(0); i++) {
			System.err.print((char)byteBuffer.get(i));
		}

		return -1;
	}
	*/
}

void Shader::bind() {
	glUseProgram(handle);
}

void Shader::dispose() {
	glDeleteProgram(handle);
}

int Shader::getUniformLocation(std::string varName) {
	int location = glGetUniformLocation(handle, varName.c_str());
	if (location < 0) {
		//System.err.println(varName + " uniform not found.");
		return -1;
	}
	else {
		//System.out.println(varName + " uniform found.");
		return location;
	}
}

int Shader::getAttribLocation(std::string varName) {
	int location = glGetAttribLocation(handle, varName.c_str());
	if (location < 0) {
		std::cout << varName << " not found." << std::endl;
		return -1;
	}
	else {
		//System.out.println(varName + " attribute found");
		return location;
	}
}

void Shader::release() {
	for (int i = 0; i < NUMBER_OF_SHADER_TYPES; ++i) {
		if (shadersHandles[i]) {
			glDetachShader(programHandle, shadersHandles[i]);
			glDeleteShader(shadersHandles[i]);
			shadersHandles[i] = 0;
		}
	}

	if (programHandle){
		glDeleteShader(programHandle);
		programHandle = 0;

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