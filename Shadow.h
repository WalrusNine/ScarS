#pragma once

#include "VertexBufferObject.h"
#include "FrameBuffer.h"

class Shadow {

public:
	Shadow();

	void update();


private:

	FrameBuffer shadowMap;
	int textureSize;

	VertexBufferObject vbo;
	unsigned int vao;

};