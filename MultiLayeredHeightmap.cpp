#include "MultiLayeredHeightmap.h"

#include <FreeImage.h>
#pragma comment (lib, "FreeImage.lib")

#include <string>
#include <vector>

MultiLayeredHeightmap::MultiLayeredHeightmap() {
	renderScale = glm::vec3(1.0f, 1.0f, 1.0f);
}

/*-----------------------------------------------

Name:	LoadHeightMapFromImage

Params:	sImagePath - path to the (optimally) grayscale
image containing heightmap data.

Result: Loads a heightmap and builds up all OpenGL
structures for rendering.

/*---------------------------------------------*/

bool MultiLayeredHeightmap::LoadHeightMapFromImage(std::string path) {
	if (isLoaded) {
		isLoaded = false;
		ReleaseHeightmap();
	}

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(path.c_str(), 0); // Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(path.c_str());

	if (fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, path.c_str());
	if (!dib)
		return false;

	BYTE* dataPointer = FreeImage_GetBits(dib); // Retrieve the image data
	rows = FreeImage_GetHeight(dib);
	columns = FreeImage_GetWidth(dib);

	// We also require our image to be either 24-bit (classic RGB) or 8-bit (luminance)
	if (dataPointer == NULL || rows == 0 || columns == 0 || (FreeImage_GetBPP(dib) != 24 && FreeImage_GetBPP(dib) != 8))
		return false;

	// How much to increase data pointer to get to next pixel data
	unsigned int ptr_inc = FreeImage_GetBPP(dib) == 24 ? 3 : 1;
	// Length of one row in data
	unsigned int row_step = ptr_inc*columns;
	
	// All vertex data are here (there are rows*columns vertices in this heightmap), we will get to normals later
	std::vector< std::vector< glm::vec3> > vertexData(rows, std::vector<glm::vec3>(columns));
	std::vector< std::vector< glm::vec2> > coordsData(rows, std::vector<glm::vec2>(columns));

	float textureU = float(columns)*0.1f;	// One texture every 10 columns
	float textureV = float(rows)*0.1f;	// One texture every 10 rows

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns; ++j) {
			float scaleC = float(j) / float(columns - 1);
			float scaleR = float(i) / float(rows - 1);
			float vertexHeight = float(*(dataPointer + row_step*i + j*ptr_inc)) / 255.0f;
			vertexData[i][j] = glm::vec3(-0.5f + scaleC, vertexHeight, -0.5f + scaleR);
			coordsData[i][j] = glm::vec2(textureU*scaleC, textureV*scaleR);
		}
	}
	/*
	// Normals are here - the heightmap contains ( (iRows-1)*(iCols-1) quads, each one containing 2 triangles, therefore array of we have 3D array)
	std::vector< std::vector<glm::vec3> > normals[2];
	normals[0] = std::vector< std::vector<glm::vec3> >(rows - 1, std::vector<glm::vec3>(columns - 1));
	normals[1] = std::vector< std::vector<glm::vec3> >(rows - 1, std::vector<glm::vec3>(columns - 1));

	for (int i = 0; i < rows - 1; ++i) {
		for (int j = 0; j < columns - 1; ++j) {
			glm::vec3 triangle0[] =
			{
				vertexData[i][j],
				vertexData[i + 1][j],
				vertexData[i + 1][j + 1]
			};
			glm::vec3 triangle1[] =
			{
				vertexData[i + 1][j + 1],
				vertexData[i][j + 1],
				vertexData[i][j]
			};

			glm::vec3 triangleNorm0 = glm::cross(triangle0[0] - triangle0[1], triangle0[1] - triangle0[2]);
			glm::vec3 triangleNorm1 = glm::cross(triangle1[0] - triangle1[1], triangle1[1] - triangle1[2]);

			normals[0][i][j] = glm::normalize(triangleNorm0);
			normals[1][i][j] = glm::normalize(triangleNorm1);
		}
	}

	std::vector< std::vector<glm::vec3> > finalNormals = std::vector< std::vector<glm::vec3> >(rows, std::vector<glm::vec3>(columns));

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns; ++j) {
			// Now we wanna calculate final normal for [i][j] vertex. We will have a look at all triangles this vertex is part of, and then we will make average vector
			// of all adjacent triangles' normals

			glm::vec3 finalNormal = glm::vec3(0.0f, 0.0f, 0.0f);

			// Look for upper-left triangles
			if (j != 0 && i != 0) {
				finalNormal += normals[0][i - 1][j - 1];
				finalNormal += normals[1][i - 1][j - 1];
			}
			// Look for upper-right triangles
			if (i != 0 && j != columns - 1) finalNormal += normals[0][i - 1][j];
			// Look for bottom-right triangles
			if (i != rows - 1 && j != columns - 1) {
				finalNormal += normals[0][i][j];
				finalNormal += normals[1][i][j];
			}
			// Look for bottom-left triangles
			if (i != rows - 1 && j != 0)
				finalNormal += normals[1][i][j - 1];

			finalNormal = glm::normalize(finalNormal);

			finalNormals[i][j] = finalNormal; // Store final normal of j-th vertex in i-th row
		}
	}
	*/
	// First, create a VBO with only vertex data
	vboHeightmapData.CreateVBO({ sizeof(glm::vec3), sizeof(glm::vec2), sizeof(glm::vec3) }); // Preallocate memory
	
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns; ++j) {
			vboHeightmapData.AddData(&vertexData[i][j], sizeof(glm::vec3)); // Add vertex
			vboHeightmapData.AddData(&coordsData[i][j], sizeof(glm::vec2)); // Add tex. coord
			//vboHeightmapData.addData(&finalNormals[i][j], sizeof(glm::vec3)); // Add normal
		}
	}
	/*
	// Now create a VBO with heightmap indices
	vboHeightmapIndices.createVBO({});

	int primitiveRestartIndex = rows*columns;
	for (int i = 0; i < rows - 1; ++i) {
		for (int j = 0; j < columns; ++j) {
			int row = i + 1;
			int index = row*columns + j;
			vboHeightmapIndices.addData(&index, sizeof(int));

			row = i;
			index = row*columns + j;
			vboHeightmapIndices.addData(&index, sizeof(int));
		}
		// Restart triangle strips
		vboHeightmapIndices.addData(&primitiveRestartIndex, sizeof(int));
	}
	*/

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// Attach vertex data to this VAO
	vboHeightmapData.BindVBO();
	vboHeightmapData.UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
	/*
	// And now attach index data to this VAO
	// Here don't forget to bind another type of VBO - the element array buffer, or simplier indices to vertices
	vboHeightmapIndices.bindVBO(GL_ELEMENT_ARRAY_BUFFER);
	vboHeightmapIndices.uploadDataToGPU(GL_STATIC_DRAW);
	*/
	isLoaded = true; // If get here, we succeeded with generating heightmap
	
	return true;
}

/*-----------------------------------------------

Name:	SetRenderSize

Params:	fRenderX, fHeight, fRenderZ - enter all 3
dimensions separately

OR

fQuadSize, fHeight - how big should be one quad
of heightmap and height is just height :)

Result: Sets rendering size (scaling) of heightmap.

/*---------------------------------------------*/

void MultiLayeredHeightmap::SetRenderSize(float renderX, float height, float renderZ) {
	renderScale = glm::vec3(renderX, height, renderZ);
}

void MultiLayeredHeightmap::SetRenderSize(float quadSize, float height) {
	renderScale = glm::vec3(float(columns) * quadSize, height, float(rows) * quadSize);
}
/*
void MultiLayeredHeightmap::RenderHeightmap() {
	spTerrain.SetUniform("fRenderHeight", renderScale.y);
	spTerrain.SetUniform("fMaxTextureU", float(columns)*0.1f);
	spTerrain.SetUniform("fMaxTextureV", float(rows)*0.1f);

	spTerrain.SetUniform("HeightmapScaleMatrix", glm::scale(glm::mat4(1.0), glm::vec3(renderScale)));

	// Now we're ready to render - we are drawing set of triangle strips using one call, but we g otta enable primitive restart
	glBindVertexArray(vao);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(rows*columns);

	int iNumIndices = (rows - 1)*columns * 2 + rows - 1;
	glDrawElements(GL_TRIANGLE_STRIP, iNumIndices, GL_UNSIGNED_INT, 0);
}
*/
/*-----------------------------------------------

Name:	ReleaseHeightmap

Params:	none

Result: Releases all data of one heightmap instance.

/*---------------------------------------------*/

void MultiLayeredHeightmap::ReleaseHeightmap() {
	if (!isLoaded)
		return; // Heightmap must be loaded

	vboHeightmapData.DeleteVBO();
	vboHeightmapIndices.DeleteVBO();
	glDeleteVertexArrays(1, &vao);
	isLoaded = false;
}

/*-----------------------------------------------

Name:	Getters

Params:	none

Result:	They get something :)

/*---------------------------------------------*/

int MultiLayeredHeightmap::GetNumHeightmapRows() {
	return rows;
}

int MultiLayeredHeightmap::GetNumHeightmapCols() {
	return columns;
}