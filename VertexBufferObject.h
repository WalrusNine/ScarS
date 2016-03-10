#pragma once

#include <GL\glew.h>

#include <vector>

class VertexBufferObject {

public:
	void createVBO(int size = 0);
	void deleteVBO();

	void* mapBufferToMemory(int usageHint);
	void* mapSubBufferToMemory(int usageHint, GLuint offset, GLuint length);
	void unmapBuffer();

	void bindVBO(int bufferType = GL_ARRAY_BUFFER);
	void uploadDataToGPU(int usageHint = GL_STATIC_DRAW);

	void addData(void* data, GLuint dataSize);

	void* getDataPointer();
	GLuint getBufferID();

	int getCurrentSize();

	VertexBufferObject();

private:
	GLuint buffer;
	int size;
	int currentSize;
	int bufferType;
	std::vector<GLbyte> data;

	bool wasDataUploaded;
};