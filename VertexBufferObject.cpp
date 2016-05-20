#include "VertexBufferObject.h"


VertexBufferObject::VertexBufferObject() {
	wasDataUploaded = false;
	buffer = 0;
}

void VertexBufferObject::CreateVBO(std::vector<int> sizes) {
	glGenBuffers(1, &buffer);
	this->infoSizes = sizes;
	this->size = 0;
	currentSize = 0;

	vertexTotalSize = 0;
	for (int i = 0; i < (int)sizes.size(); ++i) {
		vertexTotalSize += sizes[i];
	}
}

void VertexBufferObject::DeleteVBO() {
	glDeleteBuffers(1, &buffer);
	wasDataUploaded = false;
	data.clear();
}

void* VertexBufferObject::MapBufferToMemory(int usageHint) {
	if (!wasDataUploaded) return NULL;
	void* res = glMapBuffer(bufferType, usageHint);
	return res;
}


void* VertexBufferObject::MapSubBufferToMemory(int usageHint, GLuint offset, GLuint length) {
	if (!wasDataUploaded)return NULL;
	void* res = glMapBufferRange(bufferType, offset, length, usageHint);
	return res;
}

void VertexBufferObject::UnmapBuffer() {
	glUnmapBuffer(bufferType);
}

void VertexBufferObject::BindVBO(int bufferType) {
	this->bufferType = bufferType;
	glBindBuffer(bufferType, buffer);
}

void VertexBufferObject::UploadDataToGPU(int drawingHint) {
	glBufferData(bufferType, data.size(), &data[0], drawingHint);
	wasDataUploaded = true;
	data.clear();
}

void VertexBufferObject::AddData(void* d, GLuint dataSize) {
	data.insert(data.end(), (GLbyte*)d, (GLbyte*)d + dataSize);
	currentSize += dataSize;
}

void VertexBufferObject::AddVertex(void* pos, void* uv, void* normal, void* diffuse) {
	AddData(pos, infoSizes[0]);
	AddData(uv, infoSizes[1]);
	AddData(normal, infoSizes[2]);
	AddData(diffuse, infoSizes[3]);
}

void* VertexBufferObject::GetDataPointer() {
	if (wasDataUploaded) return NULL;
	return (void*)data[0];
}

GLuint VertexBufferObject::GetBufferID() {
	return buffer;
}

int VertexBufferObject::GetCurrentSize() {
	return currentSize;
}

int VertexBufferObject::GetVertexTotalSize() {
	return vertexTotalSize;
}

std::vector<int> VertexBufferObject::GetVertexSizes() {
	return infoSizes;
}