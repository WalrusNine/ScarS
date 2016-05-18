#pragma once

#include <GL\glew.h>
#include <glm/glm.hpp>
#include "VertexBufferObject.h"

class MultiLayeredHeightmap {

private:

	unsigned int vao;
	bool isLoaded;
	bool isShaderProgramLoaded;
	int rows, columns;

	glm::vec3 renderScale;

	VertexBufferObject vboHeightmapData;
	VertexBufferObject vboHeightmapIndices;

public:

	MultiLayeredHeightmap();

	bool LoadHeightMapFromImage(std::string path);
	void ReleaseHeightmap();

	void RenderHeightmap();

	void SetRenderSize(float quadSize, float height);
	void SetRenderSize(float renderX, float height, float renderZ);

	int GetNumHeightmapRows();
	int GetNumHeightmapCols();
};