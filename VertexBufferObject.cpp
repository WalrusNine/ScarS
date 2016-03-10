#include "VertexBufferObject.h"


VertexBufferObject::VertexBufferObject()
{
	wasDataUploaded = false;
	buffer = 0;
}

/*-----------------------------------------------

Name:	CreateVBO

Params:	a_iSize - initial size of buffer

Result:	Creates vertex buffer object.

/*---------------------------------------------*/

void VertexBufferObject::createVBO(int size)
{
	glGenBuffers(1, &buffer);
	data.reserve(size);
	this->size = size;
	currentSize = 0;
}

/*-----------------------------------------------

Name:	DeleteVBO

Params:	none

Result:	Releases VBO and frees all memory.

/*---------------------------------------------*/

void VertexBufferObject::deleteVBO()
{
	glDeleteBuffers(1, &buffer);
	wasDataUploaded = false;
	data.clear();
}

/*-----------------------------------------------

Name:	mapBufferToMemory

Params:	iUsageHint - GL_READ_ONLY, GL_WRITE_ONLY...

Result:	Maps whole buffer data to memory and
returns pointer to data.

/*---------------------------------------------*/

void* VertexBufferObject::mapBufferToMemory(int usageHint)
{
	if (!wasDataUploaded) return NULL;
	void* res = glMapBuffer(bufferType, usageHint);
	return res;
}

/*-----------------------------------------------

Name:	MapSubBufferToMemory

Params:	iUsageHint - GL_READ_ONLY, GL_WRITE_ONLY...
uiOffset - data offset (from where should
data be mapped).
uiLength - length of data

Result:	Maps specified part of buffer to memory.

/*---------------------------------------------*/

void* VertexBufferObject::mapSubBufferToMemory(int usageHint, GLuint offset, GLuint length)
{
	if (!wasDataUploaded)return NULL;
	void* res = glMapBufferRange(bufferType, offset, length, usageHint);
	return res;
}

/*-----------------------------------------------

Name:	UnmapBuffer

Params:	none

Result:	Unmaps previously mapped buffer.

/*---------------------------------------------*/

void VertexBufferObject::unmapBuffer()
{
	glUnmapBuffer(bufferType);
}

/*-----------------------------------------------

Name:	BindVBO

Params:	a_iBufferType - buffer type (GL_ARRAY_BUFFER, ...)

Result:	Binds this VBO.

/*---------------------------------------------*/

void VertexBufferObject::bindVBO(int a_iBufferType)
{
	bufferType = a_iBufferType;
	glBindBuffer(bufferType, buffer);
}

/*-----------------------------------------------

Name:	UploadDataToGPU

Params:	iUsageHint - GL_STATIC_DRAW, GL_DYNAMIC_DRAW...

Result:	Sends data to GPU.

/*---------------------------------------------*/

void VertexBufferObject::uploadDataToGPU(int iDrawingHint)
{
	glBufferData(bufferType, data.size(), &data[0], iDrawingHint);
	wasDataUploaded = true;
	data.clear();
}

/*-----------------------------------------------

Name:	AddData

Params:	ptrData - pointer to arbitrary data
uiDataSize - data size in bytes

Result:	Adds arbitrary data to VBO.

/*---------------------------------------------*/

void VertexBufferObject::addData(void* d, GLuint dataSize)
{
	data.insert(data.end(), (GLbyte*)d, (GLbyte*)d + dataSize);
	currentSize += dataSize;
}

/*-----------------------------------------------

Name:	GetDataPointer

Params:	none

Result:	Returns data pointer (only before uploading).

/*---------------------------------------------*/

void* VertexBufferObject::getDataPointer()
{
	if (wasDataUploaded)return NULL;
	return (void*)data[0];
}

/*-----------------------------------------------

Name:	GetBufferID

Params:	none

Result:	Returns VBO ID.

/*---------------------------------------------*/

GLuint VertexBufferObject::getBufferID()
{
	return buffer;
}

/*-----------------------------------------------

Name:	GetCurrentSize

Params:	none

Result: Returns size of data that has been added to
the buffer object.

/*---------------------------------------------*/

int VertexBufferObject::getCurrentSize()
{
	return currentSize;
}

