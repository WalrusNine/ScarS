#pragma comment(lib, "assimp.lib")

#include <GL\glew.h>
#include "Model.h"

#include <Importer.hpp>      // C++ importer interface
#include <scene.h>           // Output data structure
#include <postprocess.h>     // Post processing fla

#include <string>
#include <vector>

VertexBufferObject Model::vbo;
GLuint Model::vao;
std::vector<Texture> Model::textures;
std::vector<Model*> Model::models;

// Set vertex size (= sum of pos, tex, colors etc. sizes)
int Model::vertexTotalSize = sizeof(aiVector3D) * 2 + sizeof(aiVector2D) + sizeof(aiColor4D);

std::string GetDirectoryPath(std::string filepath)
{
	// Get directory path
	std::string directory = "";
	for (int i = (int)filepath.size() - 1; i >= 0; --i) {
		if (filepath[i] == '\\' || filepath[i] == '/') {
			directory = filepath.substr(0, i + 1);
			break;
		}
	}
	
	return directory;
}

Model::Model(std::string name) {
	isLoaded = false;
	this->name = name;

	drawMode = GL_TRIANGLES;
}

/*-----------------------------------------------

Name:	LoadModelFromFile

Params:	sFilePath - guess ^^

Result: Loads model using Assimp library.

/*---------------------------------------------*/

bool Model::LoadModelFromFile(char* filepath) {
	// Create static variables if didn't yet
	if (vbo.getBufferID() == 0) {
		vbo.createVBO();
		textures.reserve(50);
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene)	{
		//MessageBox(appMain.hWnd, "Couldn't load model ", "Error Importing Asset", MB_ICONERROR);
		return false;
	}
	
	int totalVertices = 0;

	for (int i = 0; i < (int)scene->mNumMeshes; ++i)	{
		// For each mesh
		aiMesh* mesh = scene->mMeshes[i];
		int meshFaces = mesh->mNumFaces;
		// Add mesh`s material to list
		materialIndices.push_back(mesh->mMaterialIndex);
		// Add mesh`s index to list, to know where this mesh starts in the VBO
		int sizeBefore = vbo.getCurrentSize();
		meshStartIndices.push_back(sizeBefore / vertexTotalSize);
		for (int j = 0; j < meshFaces; ++j)	{
			// For each mesh face
			const aiFace& face = mesh->mFaces[j];
			for (int k = 0; k < 3; ++k) {
				// For each vertex
				// Vertex properties to send to GPU
				aiVector3D pos = mesh->mVertices[face.mIndices[k]];
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
				aiColor4D diffuse; aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &diffuse);
				vbo.addData(&pos, sizeof(aiVector3D));
				vbo.addData(&uv, sizeof(aiVector2D));
				vbo.addData(&normal, sizeof(aiVector3D));
				vbo.addData(&diffuse, sizeof(aiColor4D));
			}
		}
		int meshVertices = mesh->mNumVertices;
		totalVertices += meshVertices;
		meshSizes.push_back((vbo.getCurrentSize() - sizeBefore) / vertexTotalSize);
	}
	numMaterials = scene->mNumMaterials;

	std::vector<int> materialRemap(numMaterials);

	// Look for the material`s textures
	for (int i = 0; i < numMaterials; ++i) {
		const aiMaterial* material = scene->mMaterials[i];
		int a = 5;
		int texIndex = 0;
		aiString path;  // filename

		if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS) {
			std::string sDir = GetDirectoryPath(filepath);
			std::string sTextureName = path.data;
			std::string sFullPath = sDir + sTextureName;
			int iTexFound = -1;
			for (int j = 0; j < (int)textures.size(); ++j) {
				if (sFullPath == textures[j].getPath()) {
					iTexFound = j;
					break;
				}
			}
			
			if (iTexFound != -1) materialRemap[i] = iTexFound;
			else {
				Texture tNew;
				int rsc = tNew.LoadTexture2D(sFullPath, true);
				if (!rsc) {
					printf("Couldn't load texture: %s\n", sFullPath.c_str());
				}
				materialRemap[i] = (int)textures.size();
				textures.push_back(tNew);
			}
		}
	}

	// Fix index
	for (int i = 0; i < (int)meshSizes.size(); ++i) {
		int iOldIndex = materialIndices[i];
		materialIndices[i] = materialRemap[iOldIndex];
	}

	return isLoaded = true;
}

/*-----------------------------------------------

Name:	FinalizeVBO

Params: none

Result: Uploads all loaded model data in one global
models' VBO.

/*---------------------------------------------*/

void Model::FinalizeVBO()
{
	// Bind VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// Bind VBO
	vbo.bindVBO();
	vbo.uploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	int loc = Shader::shader->getAttribLocation("a_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, vertexTotalSize, 0);
	// Texture coordinates
	loc = Shader::shader->getAttribLocation("a_coord");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, vertexTotalSize, (void*)sizeof(aiVector3D));
	// Normal vectors
	loc = Shader::shader->getAttribLocation("a_normal");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, vertexTotalSize, (void*)(sizeof(aiVector3D) + sizeof(aiVector2D)));
	// Color
	loc = Shader::shader->getAttribLocation("a_color");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, vertexTotalSize, (void*)(2 * sizeof(aiVector3D) + sizeof(aiVector2D)));
}

/*-----------------------------------------------

Name:	BindModelsVAO

Params: none

Result: Binds VAO of models with their VBO.

/*---------------------------------------------*/

void Model::BindModelsVAO()
{
	glBindVertexArray(vao);
}

Model* Model::createModel(char * filepath, std::string name)
{
	Model* m = new Model(name);
	m->LoadModelFromFile(filepath);

	return m;
}

Model* Model::createGeometry(Geometry g, Texture * t, std::string name)
{
	if (vbo.getBufferID() == 0) {
		vbo.createVBO();
		textures.reserve(50);
	}

	aiVector3D pos;
	aiVector2D uv;
	aiVector3D normal = aiVector3D(1.0f, 1.0f, 1.0f);
	aiColor4D diffuse(1, 1, 1, 1);
	int n = 0;

	int sizeBefore = vbo.getCurrentSize();

	// Create model
	Model* model = new Model();
	model->materialIndices.push_back(0);
	model->meshStartIndices.push_back(sizeBefore / vertexTotalSize);

	// Setup vertices
	if (g == GEOMETRY_PLANE) {
		n = 6;

		// First half
		pos = aiVector3D(-1.0f, 0, -1.0f);
		uv = aiVector2D(0, 1.0f) * 100.0f;
		vbo.addData(&pos, sizeof(aiVector3D));
		vbo.addData(&uv, sizeof(aiVector2D));
		vbo.addData(&normal, sizeof(aiVector3D));
		vbo.addData(&diffuse, sizeof(aiColor4D));

		pos = aiVector3D(1.0f, 0, -1.0f);
		uv = aiVector2D(1.0f, 1.0f) * 100.0f;
		vbo.addData(&pos, sizeof(aiVector3D));
		vbo.addData(&uv, sizeof(aiVector2D));
		vbo.addData(&normal, sizeof(aiVector3D));
		vbo.addData(&diffuse, sizeof(aiColor4D));

		pos = aiVector3D(1.0f, 0, 1.0f);
		uv = aiVector2D(1.0f, 0) * 100.0f;
		vbo.addData(&pos, sizeof(aiVector3D));
		vbo.addData(&uv, sizeof(aiVector2D));
		vbo.addData(&normal, sizeof(aiVector3D));
		vbo.addData(&diffuse, sizeof(aiColor4D));

		// Second half
		pos = aiVector3D(1.0f, 0, 1.0f);
		uv = aiVector2D(1.0f, 0) * 100.0f;
		vbo.addData(&pos, sizeof(aiVector3D));
		vbo.addData(&uv, sizeof(aiVector2D));
		vbo.addData(&normal, sizeof(aiVector3D));
		vbo.addData(&diffuse, sizeof(aiColor4D));

		pos = aiVector3D(-1.0f, 0, 1.0f);
		uv = aiVector2D(0, 0) * 100.0f;
		vbo.addData(&pos, sizeof(aiVector3D));
		vbo.addData(&uv, sizeof(aiVector2D));
		vbo.addData(&normal, sizeof(aiVector3D));
		vbo.addData(&diffuse, sizeof(aiColor4D));

		pos = aiVector3D(-1.0f, 0, -1.0f);
		uv = aiVector2D(0, 1.0f) * 100.0f;
		vbo.addData(&pos, sizeof(aiVector3D));
		vbo.addData(&uv, sizeof(aiVector2D));
		vbo.addData(&normal, sizeof(aiVector3D));
		vbo.addData(&diffuse, sizeof(aiColor4D));
	}
	model->meshSizes.push_back((vbo.getCurrentSize() - sizeBefore) / vertexTotalSize);

	// Texture
	int texLocation = -1;
	for (int i = 0; i < (int)textures.size(); ++i) {
		if (t->getTextureHandle() == textures[i].getTextureHandle()) {
			texLocation = i;
			break;
		}
	}
	if (texLocation != -1) {
		model->materialIndices[0] = texLocation;
	}
	else {
		model->materialIndices[0] = textures.size();
		textures.push_back(*t);
	}
	
	model->name = name;
	model->isLoaded = true;



	return model;
}

Model * Model::createSkybox(std::string dir, std::vector<std::string> filenames, std::string name)
{
	if (vbo.getBufferID() == 0) {
		vbo.createVBO();
		textures.reserve(50);
	}

	int totalVertices = 0;

	// Create model
	Model* model = new Model();

	// Load textures
	std::vector<Texture> sbtextures(6);

	for (int i = 0; i < 6; ++i) {
		sbtextures[i].LoadTexture2D(dir + filenames[i]);
	}

	// Set textures parameters
	for (int i = 0; i < 6; ++i) {
		sbtextures[i].setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
		sbtextures[i].setSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		sbtextures[i].setSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	// Setup vertices
	aiVector3D vSkyBoxVertices[24] =
	{
		// Front face
		aiVector3D(200.0f, 200.0f, 200.0f), aiVector3D(200.0f, -200.0f, 200.0f), aiVector3D(-200.0f, 200.0f, 200.0f), aiVector3D(-200.0f, -200.0f, 200.0f),
		// Back face
		aiVector3D(-200.0f, 200.0f, -200.0f), aiVector3D(-200.0f, -200.0f, -200.0f), aiVector3D(200.0f, 200.0f, -200.0f), aiVector3D(200.0f, -200.0f, -200.0f),
		// Left face
		aiVector3D(-200.0f, 200.0f, 200.0f), aiVector3D(-200.0f, -200.0f, 200.0f), aiVector3D(-200.0f, 200.0f, -200.0f), aiVector3D(-200.0f, -200.0f, -200.0f),
		// Right face
		aiVector3D(200.0f, 200.0f, -200.0f), aiVector3D(200.0f, -200.0f, -200.0f), aiVector3D(200.0f, 200.0f, 200.0f), aiVector3D(200.0f, -200.0f, 200.0f),
		// Top face
		aiVector3D(-200.0f, 200.0f, -200.0f), aiVector3D(200.0f, 200.0f, -200.0f), aiVector3D(-200.0f, 200.0f, 200.0f), aiVector3D(200.0f, 200.0f, 200.0f),
		// Bottom face
		aiVector3D(200.0f, -200.0f, -200.0f), aiVector3D(-200.0f, -200.0f, -200.0f), aiVector3D(200.0f, -200.0f, 200.0f), aiVector3D(-200.0f, -200.0f, 200.0f),
	};
	aiVector2D vSkyBoxTexCoords[4] =
	{
		aiVector2D(0.0f, 1.0f), aiVector2D(0.0f, 0.0f), aiVector2D(1.0f, 1.0f), aiVector2D(1.0f, 0.0f)
	};

	aiVector3D vSkyBoxNormals[6] =
	{
		aiVector3D(0.0f, 0.0f, -1.0f),
		aiVector3D(0.0f, 0.0f, 1.0f),
		aiVector3D(1.0f, 0.0f, 0.0f),
		aiVector3D(-1.0f, 0.0f, 0.0f),
		aiVector3D(0.0f, -1.0f, 0.0f),
		aiVector3D(0.0f, 1.0f, 0.0f)
	};

	// Make it into a model

	// Meshes
	for (int i = 0; i < 6; ++i) {
		model->materialIndices.push_back(i);
		int sizeBefore = vbo.getCurrentSize();
		model->meshStartIndices.push_back(sizeBefore / vertexTotalSize);
		// MeshFaces
		for (int j = 0; j < 4; ++j) {
			//Vertices
			aiVector3D pos = vSkyBoxVertices[i*4 + j];
			aiVector2D uv = vSkyBoxTexCoords[j];
			aiVector3D normal = vSkyBoxNormals[i];
			aiColor4D diffuse(1, 1, 1, 1);
			vbo.addData(&pos, sizeof(aiVector3D));
			vbo.addData(&uv, sizeof(aiVector2D));
			vbo.addData(&normal, sizeof(aiVector3D));
			vbo.addData(&diffuse, sizeof(aiColor4D));
		}
		int meshVertices = 4;
		totalVertices += meshVertices;
		model->meshSizes.push_back((vbo.getCurrentSize() - sizeBefore) / vertexTotalSize);

	}

	std::vector<int> materialRemap((int)model->meshSizes.size());
	// Texture
	for (int i = 0; i < (int)sbtextures.size(); ++i) {
		int texLocation = -1;
		for (int j = 0; j < (int)textures.size(); ++j) {
			if (sbtextures[i].getTextureHandle() == textures[j].getTextureHandle()) {
				texLocation = j;
				break;
			}
		}
		if (texLocation != -1) {
			materialRemap[i] = texLocation;
		}
		else {
			materialRemap[i] = textures.size();
			textures.push_back(sbtextures[i]);
		}
	}

	for (int i = 0; i < (int)model->meshSizes.size(); ++i) {
		int iOldIndex = model->materialIndices[i];
		model->materialIndices[i] = materialRemap[iOldIndex];
	}

	model->name = name;
	model->isLoaded = true;

	model->drawMode = GL_TRIANGLE_STRIP;

	return model;

}

void Model::addModel(Model * m)
{
	models.push_back(m);
}

/*-----------------------------------------------

Name:	RenderModel

Params: none

Result: Guess what it does ^^.

/*---------------------------------------------*/

void Model::render()
{
	if (!isLoaded) return;
	int numMeshes = (int)meshSizes.size();
	for (int i = 0; i < numMeshes; ++i) {
		int matIndex = materialIndices[i];
		textures[matIndex].bind(0);
		glDrawArrays(drawMode, meshStartIndices[i], meshSizes[i]);
	}
}

Model* Model::getModel(std::string name)
{
	int len = models.size();

	for (int i = 0; i < len; ++i) {
		if (models[i]->name == name) {
			return models[i];
		}
	}

	return nullptr;
}

std::string GeometryToString(Geometry g) {
	switch (g) {
		case GEOMETRY_NONE:
			return "none";
		case GEOMETRY_PLANE:
			return "plane";
		case GEOMETRY_CUBE:
			return "cube";
		case GEOMETRY_PYRAMID:
			return "pyramid";
		case GEOMETRY_SPHERE:
			return "sphere";
		default: return "none";
	}
}

std::string Model::getName() {
	return name;
}