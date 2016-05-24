#include "Scene.h"

//Include GLEW  
#include <GL/glew.h>  
//Include GLFW  
#include <GLFW/glfw3.h>

#include <texture.h>

#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"
#include "SkyCamera.h"
#include "Texture.h"
#include "VertexBufferObject.h"
#include "Model.h"
#include "House.h"
#include "Car.h"
#include "Wolf.h"
#include "Skybox.h"
#include "Player.h"
#include "GUI.h"
#include "SpotLight.h"
#include "Sun.h"
#include "FrameBuffer.h"
#include "MultiLayeredHeightmap.h"
#include "Television.h"
#include "Shadow.h"
#include "Fog.h"
#include "FreeTypeFont.h"
#include "ParticleSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Shadow* shadow;
ParticleSystem* ps;

Scene::Scene() {
	init();
}

void Scene::init() {
	GameObject::gameObjects.reserve(1);
	Camera::mainCamera = new Camera();
	GameObject::gameObjects.push_back(Camera::mainCamera);

	// Create Textures
	Texture::AddTexture(Texture::CreateTexture("data\\textures\\golddiag.jpg", TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, "golddiag", true));
	Texture::AddTexture(Texture::CreateTexture("data\\textures\\snow.jpg", TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, "snow", true));
	Texture::AddTexture(Texture::CreateTexture("data\\textures\\box.jpg", TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, "box", true));
	Texture::AddTexture(Texture::CreateTexture("data\\textures\\particle.bmp", TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, "particle", true));

	// Create Models
	Model::addModel(Model::createModel("data\\models\\FireGTO\\FireGTO.blend", "car"));
	Model::addModel(Model::createModel("data\\models\\house\\house.3ds", "house"));
	Model::addModel(Model::createModel("data\\models\\Wolf\\Wolf.obj", "wolf"));
	Model::addModel(Model::createModel("data\\models\\tree\\Tree1.3ds", "tree"));
	Model::addModel(Model::createModel("data\\models\\building\\Small_Building_1.blend", "building"));
	Model::addModel(Model::createModel("data\\models\\Roads\\Roads.blend", "roads"));
	Model::addModel(Model::createGeometry(GEOMETRY_PLANE, Texture::GetTexture("snow"), vec4(1.0f), "ground"));
	Model::addModel(Model::createGeometry(GEOMETRY_CUBE, Texture::GetTexture("box"), vec4(1.0f), "woodenbox"));
	Model::addModel(Model::createGeometry(GEOMETRY_CUBE, nullptr, vec4(0.0f, 0.0f, 1.0f, 0.5f), "colorbox"));
	Model::addModel(Model::createGeometry(GEOMETRY_PLANE, nullptr, vec4(1.0f, 1.0f, 1.0f, 1.0f), "panel"));
	Model::addModel(Model::createGeometry(GEOMETRY_PLANE, nullptr, vec4(1.0f, 1.0f, 1.0f, 1.0f), "water"));
	//Model::addModel(Model::createSkybox("data\\skyboxes\\ame_siege\\", { "siege_ft.tga", "siege_bk.tga", "siege_lf.tga", "siege_rt.tga", "siege_up.tga", "siege_dn.tga" }, "siege"));
	//Model::addModel(Model::createSkybox("data\\skyboxes\\elbrus\\", { "elbrus_front.tga", "elbrus_back.tga", "elbrus_right.tga", "elbrus_left.tga", "elbrus_top.tga", "elbrus_top.tga" }, "siege"));
	Model::addModel(Model::createSkybox("data\\skyboxes\\jajlands1\\", { "jajlands1_ft.jpg", "jajlands1_bk.jpg", "jajlands1_lf.jpg", "jajlands1_rt.jpg", "jajlands1_up.jpg", "jajlands1_dn.jpg" }, "jajlands1"));
	Model::FinalizeVBO();
	
	// Create GameObjects
	GameObject* sbo = new Skybox();
	GameObject::skyBox = sbo;
	sbo->model = Model::getModel("jajlands1");
	GameObject::gameObjects.push_back(sbo);

	for (int i = 0; i < 6; ++i) {
		// Houses
		GameObject* go = new House();
		go->position.x = -80.0f + i * 30.0f;
		go->position.y = -9.8f;
		go->position.z = -8;
		GameObject::gameObjects.push_back(go);

		// Cars
		go = new Car();
		go->position.x = -65.0f + i * 30.0f;
		go->position.y = -9.8f;
		go->position.z = -8;
		GameObject::gameObjects.push_back(go);
	}

	// Wolf
	GameObject* wolf = new Wolf();
	wolf->position.y = -9.8f;
	GameObject::gameObjects.push_back(wolf);

	// Tree
	GameObject* tree = new GameObject("tree");
	tree->model = Model::getModel("tree");
	tree->position.z = 15;
	tree->position.y = -9.8f;
	tree->rotation.x = 270.0f;
	GameObject::gameObjects.push_back(tree);

	// Building
	GameObject* building = new GameObject("building");
	building->model = Model::getModel("building");
	building->position.z = 60.f;
	building->position.y = -9.8f;
	building->rotation.x = 270.0f;
	building->scale *= 10.0f;
	building->rotation.z = 180.0f;
	GameObject::gameObjects.push_back(building);

	// Box
	GameObject* box = new GameObject("box");
	box->model = Model::getModel("woodenbox");
	box->scale *= 0.6f;
	box->position.z = 15.0f;
	box->position.y = -9.2f;
	GameObject::gameObjects.push_back(box);

	// Colored box
	GameObject* colorBox = new GameObject("blendbox");
	colorBox->model = Model::getModel("colorbox");
	colorBox->model->setTexture(Texture::GetTexture("box"));
	colorBox->position.x = -20.0f;
	colorBox->position.y = -4.0f;
	colorBox->position.z = 20.0f;
	colorBox->scale.y *= 6.0f;
	colorBox->scale.z *= 3;
	colorBox->setTransparent(true);
	GameObject::gameObjects.push_back(colorBox);

	// Ground
	GameObject* ground = new GameObject("ground");
	ground->model = Model::getModel("ground");
	ground->scale = vec3(100, 0, 100);
	ground->position = vec3(0, -10.1f, 0);
	GameObject::gameObjects.push_back(ground);

	// Player
	GameObject* player = new Player();
	GameObject::gameObjects.push_back(player);

	// Sun
	GameObject::sun = new Sun();
	GameObject::gameObjects.push_back(GameObject::sun);

	// Roads
	GameObject* roads = new GameObject("road");
	roads->model = Model::getModel("roads");
	roads->position.x = -20;
	roads->position.y = -9.8f;
	roads->position.z = 20;
	roads->rotation.x = 270;
	roads->rotation.z = 90;
	roads->scale.x *= 20;
	roads->scale.y *= 100;
	GameObject::gameObjects.push_back(roads);

	Television* television = new Television();
	television->create();
	GameObject::gameObjects.push_back(television);

	GameObject* tempBox = new GameObject("tempbox");
	tempBox->model = Model::getModel("woodenbox");
	tempBox->position = television->position + glm::vec3(.0f, .0f, 3.0f);
	//GameObject::gameObjects.push_back(tempBox);

	Fog* fog = new Fog();
	fog->init();
	GameObject::gameObjects.push_back(fog);

	// Organize list (transparent objects last)
	int len = (int)GameObject::gameObjects.size();
	for (int i = 0; i < len; ++i) {
		if (GameObject::gameObjects[i]->isTransparent()) {
			GameObject* tr = GameObject::gameObjects[i];
			GameObject::gameObjects.erase(GameObject::gameObjects.begin() + i);
			GameObject::gameObjects.push_back(tr);
		}
	}

	// GUI
	GUI* gui = new GUI();
	gui->SetOrtho2D();
	GameObject::gameObjects.push_back(gui);

	// SHADOW
	shadow = new Shadow();

	ps = new ParticleSystem();
	ps->InitalizeParticleSystem();

	ps->SetGeneratorProperties(
		glm::vec3(-10.0f, 17.5f, 0.0f), // Where the particles are generated
		glm::vec3(-5, 0, -5), // Minimal velocity
		glm::vec3(5, 20, 5), // Maximal velocity
		glm::vec3(0, -5, 0), // Gravity force applied to particles
		glm::vec3(0.01f, 0.01f, 0.01f), // Color
		1.5f, // Minimum lifetime in seconds
		3.0f, // Maximum lifetime in seconds
		0.75f, // Rendered size
		0.02f, // Spawn every 0.05 seconds
		30); // And spawn 30 particles
}

void Scene::render() {
	// Shadow
	shadow->update();

	// AssImp
	Model::BindModelsVAO();
	
	// Update objects
	int len = GameObject::gameObjects.size();
	for (int i = 0; i < len; ++i) {
		GameObject::gameObjects[i]->update();
	}

	// Draw objects
	for (int i = 0; i < len; ++i) {
		GameObject::gameObjects[i]->draw();
	}


	// Particles
	// Bind texture
	Texture::GetTexture("particle")->Bind();
	ps->SetMatrices(&Camera::mainCamera->getProjectionMatrix(), Camera::mainCamera->getPosition(), Camera::mainCamera->getViewVector(), Camera::mainCamera->getUp());
	ps->UpdateParticles(FPSController::getDeltaTime());
	ps->RenderParticles();
}

Scene::~Scene() {
	// Delete gameobjects
	int len = GameObject::gameObjects.size();
	for (int i = 0; i < len; ++i) {
		delete GameObject::gameObjects[i];
	}

	delete shadow;

	// Unload all models
	Model::ClearModels();
}