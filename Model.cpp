#pragma comment(lib, "assimp.lib")

#include <GL\glew.h>
#include "Model.h"
#include "Material.h"

#include <Importer.hpp>      // C++ importer interface
#include <scene.h>           // Output data structure
#include <postprocess.h>     // Post processing fla

#include <string>
#include <vector>

#include <iostream>

VertexBufferObject Model::vbo;
GLuint Model::vao;
std::vector<Model*> Model::models;

glm::vec3 getGlmVector(aiVector3D v) {
	return glm::vec3(v.x, v.y, v.z);
}

glm::vec2 getGlmVector(aiVector2D v) {
	return glm::vec2(v.x, v.y);
}

glm::vec4 getGlmVector(aiColor4D v) {
	return glm::vec4(v.r, v.g, v.b, v.a);
}

std::string GetDirectoryPath(std::string filepath) {
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

void Model::ClearModels() {
	vbo.deleteVBO();
	glDeleteVertexArrays(1, &vao);

	for (int i = 0; i < (int)Texture::textures.size(); ++i) {
		Texture::textures[i]->UnloadTexture();
	}
}

bool Model::LoadModelFromFile(char* filepath) {
	// Create static variables if didn't yet
	if (vbo.getBufferID() == 0) {
		vbo.createVBO({ sizeof(glm::vec3), sizeof(glm::vec2), sizeof(glm::vec3), sizeof(glm::vec4) });
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
		meshStartIndices.push_back(sizeBefore / vbo.getVertexTotalSize());
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
				//aiColor4D specular; aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_SPECULAR, &specular);
				vbo.addVertex(&getGlmVector(pos), &getGlmVector(uv), &getGlmVector(normal), &getGlmVector(diffuse));
			}
		}
		int meshVertices = mesh->mNumVertices;
		totalVertices += meshVertices;
		meshSizes.push_back((vbo.getCurrentSize() - sizeBefore) / vbo.getVertexTotalSize());
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
			int nTex = (int)Texture::textures.size();
			for (int j = 0; j < nTex; ++j) {
				if (sFullPath == Texture::textures[j]->getPath()) {
					iTexFound = j;
					break;
				}
			}
			
			if (iTexFound != -1) materialRemap[i] = iTexFound;
			else {
				Texture* tNew = new Texture();
				int rsc = tNew->LoadTexture2D(sFullPath, true);
				if (!rsc) {
					printf("Couldn't load texture: %s\n", sFullPath.c_str());
				}
				materialRemap[i] = nTex;
				Texture::addTexture(tNew);
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

void Model::FinalizeVBO() {
	int totalSize = vbo.getVertexTotalSize();
	std::vector<int> sizes = vbo.getVertexSizes();

	// Bind VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// Bind VBO
	vbo.bindVBO();
	vbo.uploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	int loc = Shader::shader->getAttribLocation("a_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, totalSize, 0);
	// Texture coordinates
	loc = Shader::shader->getAttribLocation("a_coord");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, totalSize, (void*)sizes[0]);
	// Normal vectors
	loc = Shader::shader->getAttribLocation("a_normal");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, totalSize, (void*)(sizes[0] + sizes[1]));
	// Diffuse Color
	loc = Shader::shader->getAttribLocation("a_color");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, totalSize, (void*)(sizes[0] + sizes[1] + sizes[2]));
	// Specular Color
	//loc = Shader::shader->getAttribLocation("a_spec");
	//glEnableVertexAttribArray(loc);
	//glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, vertexTotalSize, (void*)(2 * sizeof(aiVector3D) + sizeof(aiVector2D) + sizeof(aiColor4D)));
}

void Model::BindModelsVAO() {
	glBindVertexArray(vao);
}

Model* Model::createModel(char * filepath, std::string name) {
	Model* m = new Model(name);
	m->LoadModelFromFile(filepath);

	return m;
}

Model* Model::createGeometry(GeometryType g, Texture * t, glm::vec4 color, std::string name) {
	if (vbo.getBufferID() == 0) {
		vbo.createVBO({ sizeof(glm::vec3), sizeof(glm::vec2), sizeof(glm::vec3), sizeof(glm::vec4) });
	}

	std::vector<glm::vec3> pos;
	std::vector<glm::vec2> uv;

	glm::vec3 normal(1.0f, 1.0f, 1.0f);
	glm::vec4 diffuse(color.r, color.g, color.b, color.a);

	int n = 0;

	int sizeBefore = vbo.getCurrentSize();

	// Create model
	Model* model = new Model();
	model->materialIndices.push_back(0);
	model->meshStartIndices.push_back(sizeBefore / vbo.getVertexTotalSize());

	// Setup vertices
	if (g == GEOMETRY_PLANE) {
		n	= 6;
		pos = Geometry::getGeometryVertexPosition("plane");
		uv	= Geometry::getGeometryVertexUV("plane");
	}
	else if (g == GEOMETRY_CUBE) {
		n	= 36;
		pos = Geometry::getGeometryVertexPosition("cube");
		uv	= Geometry::getGeometryVertexUV("cube");
	}

	for (int i = 0; i < n; ++i) {
		vbo.addVertex(&pos[i], &uv[i], &normal, &diffuse);
	}


	model->meshSizes.push_back((vbo.getCurrentSize() - sizeBefore) / vbo.getVertexTotalSize());

	if (t != nullptr) {
		// Texture
		int texLocation = -1;
		int nTex = (int)Texture::textures.size();
		for (int i = 0; i < nTex; ++i) {
			if (t->getTextureHandle() == Texture::textures[i]->getTextureHandle()) {
				texLocation = i;
				break;
			}
		}
		if (texLocation != -1) {
			model->materialIndices[0] = texLocation;
		}
		else {
			model->materialIndices[0] = nTex;
			Texture::textures.push_back(t);
		}
	}
	
	model->name = name;
	model->isLoaded = true;



	return model;
}

void Model::setTexture(Texture* t, GLenum drawmode) {
	// Texture
	int texLocation = -1;
	int nTex = (int)Texture::textures.size();
	for (int i = 0; i < nTex; ++i) {
		if (t->getTextureHandle() == Texture::textures[i]->getTextureHandle()) {
			texLocation = i;
			break;
		}
	}
	if (texLocation != -1) {
		materialIndices[0] = texLocation;
	}
	else {
		materialIndices[0] = nTex;
		Texture::textures.push_back(t);
	}

	drawMode = drawmode;
}

Model* Model::createSkybox(std::string dir, std::vector<std::string> filenames, std::string name) {
	if (vbo.getBufferID() == 0) {
		vbo.createVBO({ sizeof(glm::vec3), sizeof(glm::vec2), sizeof(glm::vec3), sizeof(glm::vec4) });
	}

	int totalVertices = 0;

	// Create model
	Model* model = new Model();

	// Load textures
	std::vector<Texture*> sbtextures(6);

	for (int i = 0; i < 6; ++i) {
		sbtextures[i] = new Texture();
		sbtextures[i]->LoadTexture2D(dir + filenames[i]);
	}

	// Set textures parameters
	for (int i = 0; i < 6; ++i) {
		sbtextures[i]->setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
		sbtextures[i]->setSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		sbtextures[i]->setSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	// Setup vertices
	glm::vec3 vSkyBoxVertices[24] =
	{
		// Front face
		glm::vec3(200.0f, 200.0f, 200.0f), glm::vec3(200.0f, -200.0f, 200.0f), glm::vec3(-200.0f, 200.0f, 200.0f), glm::vec3(-200.0f, -200.0f, 200.0f),
		// Back face
		glm::vec3(-200.0f, 200.0f, -200.0f), glm::vec3(-200.0f, -200.0f, -200.0f), glm::vec3(200.0f, 200.0f, -200.0f), glm::vec3(200.0f, -200.0f, -200.0f),
		// Left face
		glm::vec3(-200.0f, 200.0f, 200.0f), glm::vec3(-200.0f, -200.0f, 200.0f), glm::vec3(-200.0f, 200.0f, -200.0f), glm::vec3(-200.0f, -200.0f, -200.0f),
		// Right face
		glm::vec3(200.0f, 200.0f, -200.0f), glm::vec3(200.0f, -200.0f, -200.0f), glm::vec3(200.0f, 200.0f, 200.0f), glm::vec3(200.0f, -200.0f, 200.0f),
		// Top face
		glm::vec3(-200.0f, 200.0f, -200.0f), glm::vec3(200.0f, 200.0f, -200.0f), glm::vec3(-200.0f, 200.0f, 200.0f), glm::vec3(200.0f, 200.0f, 200.0f),
		// Bottom face
		glm::vec3(200.0f, -200.0f, -200.0f), glm::vec3(-200.0f, -200.0f, -200.0f), glm::vec3(200.0f, -200.0f, 200.0f), glm::vec3(-200.0f, -200.0f, 200.0f),
	};
	glm::vec2 vSkyBoxTexCoords[4] =
	{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};

	glm::vec3 vSkyBoxNormals[6] =
	{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	// Make it into a model

	// Meshes
	for (int i = 0; i < 6; ++i) {
		model->materialIndices.push_back(i);
		int sizeBefore = vbo.getCurrentSize();
		model->meshStartIndices.push_back(sizeBefore / vbo.getVertexTotalSize());
		// MeshFaces
		for (int j = 0; j < 4; ++j) {
			//Vertices
			glm::vec3 pos = vSkyBoxVertices[i*4 + j];
			glm::vec2 uv = vSkyBoxTexCoords[j];
			glm::vec3 normal = vSkyBoxNormals[i];
			glm::vec4 diffuse(1, 1, 1, 1);
			aiColor4D specular(1, 1, 1, 1);
			vbo.addVertex(&pos, &uv, &normal, &diffuse);
		}
		int meshVertices = 4;
		totalVertices += meshVertices;
		model->meshSizes.push_back((vbo.getCurrentSize() - sizeBefore) / vbo.getVertexTotalSize());

	}

	std::vector<int> materialRemap((int)model->meshSizes.size());
	
	// Texture
	for (int i = 0; i < (int)sbtextures.size(); ++i) {
		int texLocation = -1;

		int nTex = (int)Texture::textures.size();
		for (int j = 0; j < nTex; ++j) {
			if (sbtextures[i]->getTextureHandle() == Texture::textures[j]->getTextureHandle()) {
				texLocation = j;
				break;
			}
		}
		if (texLocation != -1) {
			materialRemap[i] = texLocation;
		}
		else {
			materialRemap[i] = nTex;
			Texture::textures.push_back(sbtextures[i]);
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

void Model::addModel(Model * m) {
	models.push_back(m);
}

void Model::render() {
	if (!isLoaded) return;
	int numMeshes = (int)meshSizes.size();
	for (int i = 0; i < numMeshes; ++i) {
		int matIndex = materialIndices[i];
		Texture::textures[matIndex]->bind(0);
		glDrawArrays(drawMode, meshStartIndices[i], meshSizes[i]);
	}
}

Model* Model::getModel(std::string name) {
	int len = models.size();

	for (int i = 0; i < len; ++i) {
		if (models[i]->name == name) {
			return models[i];
		}
	}

	return nullptr;
}

std::string GeometryToString(GeometryType g) {
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