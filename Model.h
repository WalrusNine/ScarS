#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Texture.h"
#include "VertexBufferObject.h"

enum Geometry
{
	GEOMETRY_NONE,
	GEOMETRY_PLANE,
	GEOMETRY_CUBE,
	GEOMETRY_PYRAMID,
	GEOMETRY_SPHERE
};

class Model
{
public:
	bool LoadModelFromFile(char* filepath);
	bool LoadModelFromFileBumpMap(char* filepath, char* colorMap, char* normalMap);

	static void FinalizeVBO();
	static void BindModelsVAO();

	static Model* createModel(char* filepath, std::string name);
	static Model* createGeometry(Geometry g, Texture* t, std::string name);
	static Model* createSkybox(std::string dir, std::vector<std::string> paths, std::string name);

	static void addModel(Model* m);

	void RenderModelBumpMap(Shader* s);
	static std::vector<Model*> models;
	void render();

	static Model* getModel(std::string name);
	std::string getName();

	Model(std::string name = "");
private:
	bool isLoaded;

	static VertexBufferObject vbo;
	static unsigned int vao;
	static std::vector<Texture> textures;
	static int vertexTotalSize;

	std::vector<int> meshStartIndices;
	std::vector<int> meshSizes;
	std::vector<int> materialIndices;
	int numMaterials;

	std::string name;

	GLenum drawMode;
};