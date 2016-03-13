#pragma once

#include <GL\glew.h>

#include <vector>

// Put static vertexTotalSize

class VertexBufferObject {

public:
	void createVBO(std::vector<int> sizes);
	void deleteVBO();

	void* mapBufferToMemory(int usageHint);
	void* mapSubBufferToMemory(int usageHint, GLuint offset, GLuint length);
	void unmapBuffer();

	void bindVBO(int bufferType = GL_ARRAY_BUFFER);
	void uploadDataToGPU(int usageHint = GL_STATIC_DRAW);

	void addData(void* data, GLuint dataSize);
	void addVertex(void* pos, void* uv, void* normal, void* diffuse);

	void* getDataPointer();
	GLuint getBufferID();

	int getCurrentSize();

	VertexBufferObject();

	int getVertexTotalSize();
	std::vector<int> getVertexSizes();

private:
	GLuint buffer;
	int size;
	int currentSize;
	int bufferType;
	std::vector<GLbyte> data;

	bool wasDataUploaded;

	// Vertex Information
	int vertexTotalSize;
	std::vector<int> infoSizes;
};