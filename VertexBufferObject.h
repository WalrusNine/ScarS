#pragma once

#include <GL/glew.h>

#include <vector>

// Put static vertexTotalSize

class VertexBufferObject {

public:
	void CreateVBO(std::vector<int> sizes);
	void DeleteVBO();

	void* MapBufferToMemory(int usageHint);
	void* MapSubBufferToMemory(int usageHint, GLuint offset, GLuint length);
	void UnmapBuffer();

	void BindVBO(int bufferType = GL_ARRAY_BUFFER);
	void UploadDataToGPU(int usageHint = GL_STATIC_DRAW);

	void AddData(void* data, GLuint dataSize);
	void AddVertex(void* pos, void* uv, void* normal, void* diffuse);

	void* GetDataPointer();
	GLuint GetBufferID();

	int GetCurrentSize();

	VertexBufferObject();

	int GetVertexTotalSize();
	std::vector<int> GetVertexSizes();

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
