#include "VertexBufferObject.h"


VertexBufferObject::VertexBufferObject() {
	wasDataUploaded = false;
	buffer = 0;
}

void VertexBufferObject::createVBO(std::vector<int> sizes) {
	glGenBuffers(1, &buffer);
	this->infoSizes = sizes;
	this->size = 0;
	currentSize = 0;

	vertexTotalSize = 0;
	for (int i = 0; i < (int)sizes.size(); ++i) {
		vertexTotalSize += sizes[i];
	}
}

void VertexBufferObject::deleteVBO() {
	glDeleteBuffers(1, &buffer);
	wasDataUploaded = false;
	data.clear();
}

void* VertexBufferObject::mapBufferToMemory(int usageHint) {
	if (!wasDataUploaded) return NULL;
	void* res = glMapBuffer(bufferType, usageHint);
	return res;
}


void* VertexBufferObject::mapSubBufferToMemory(int usageHint, GLuint offset, GLuint length) {
	if (!wasDataUploaded)return NULL;
	void* res = glMapBufferRange(bufferType, offset, length, usageHint);
	return res;
}

void VertexBufferObject::unmapBuffer() {
	glUnmapBuffer(bufferType);
}

void VertexBufferObject::bindVBO(int bufferType) {
	this->bufferType = bufferType;
	glBindBuffer(bufferType, buffer);
}

void VertexBufferObject::uploadDataToGPU(int drawingHint) {
	glBufferData(bufferType, data.size(), &data[0], drawingHint);
	wasDataUploaded = true;
	data.clear();
}

void VertexBufferObject::addData(void* d, GLuint dataSize) {
	data.insert(data.end(), (GLbyte*)d, (GLbyte*)d + dataSize);
	currentSize += dataSize;
}

void VertexBufferObject::addVertex(void* pos, void* uv, void* normal, void* diffuse) {
	addData(pos, infoSizes[0]);
	addData(uv, infoSizes[1]);
	addData(normal, infoSizes[2]);
	addData(diffuse, infoSizes[3]);
}

void* VertexBufferObject::getDataPointer() {
	if (wasDataUploaded) return NULL;
	return (void*)data[0];
}

GLuint VertexBufferObject::getBufferID() {
	return buffer;
}

int VertexBufferObject::getCurrentSize() {
	return currentSize;
}

int VertexBufferObject::getVertexTotalSize() {
	return vertexTotalSize;
}

std::vector<int> VertexBufferObject::getVertexSizes() {
	return infoSizes;
}