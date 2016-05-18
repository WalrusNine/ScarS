#include "ObjectTests.h"

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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


std::vector<GameObject*> gameObjects;
GUI* gui;
GameObject* sun;
Television* television;

void TestSetup3D() {
	// Create Camera
	//Camera::mainCamera = Camera::createCamera();
	//gameObjects.push_back((GameObject*)Camera::mainCamera);
	SkyCamera::mainCamera = SkyCamera::createCamera();
	gameObjects.push_back((GameObject*)SkyCamera::mainCamera);

	// Create Textures
	Texture::addTexture(Texture::createTexture("data\\textures\\golddiag.jpg", TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, "golddiag", true));
	Texture::addTexture(Texture::createTexture("data\\textures\\snow.jpg", TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, "snow", true));
	Texture::addTexture(Texture::createTexture("data\\textures\\box.jpg", TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, "box", true));

	// Create Models
	Model::addModel(Model::createModel("data\\models\\FireGTO\\FireGTO.blend", "car"));
	Model::addModel(Model::createModel("data\\models\\house\\house.3ds", "house"));
	Model::addModel(Model::createModel("data\\models\\Wolf\\Wolf.obj", "wolf"));
	Model::addModel(Model::createModel("data\\models\\tree\\Tree1.3ds", "tree"));
	Model::addModel(Model::createModel("data\\models\\building\\Small_Building_1.blend", "building"));
	Model::addModel(Model::createModel("data\\models\\Roads\\Roads.blend", "roads"));
	Model::addModel(Model::createGeometry(GEOMETRY_PLANE, Texture::getTexture("snow"), vec4(1.0f), "ground"));
	Model::addModel(Model::createGeometry(GEOMETRY_CUBE, Texture::getTexture("box"), vec4(1.0f), "woodenbox"));
	Model::addModel(Model::createGeometry(GEOMETRY_CUBE, nullptr, vec4(0.0f, 0.0f, 1.0f, 0.5f), "colorbox"));
	Model::addModel(Model::createGeometry(GEOMETRY_PLANE, nullptr, vec4(1.0f, 1.0f, 1.0f, 1.0f), "panel"));
	//Model::addModel(Model::createModel("data\\models\\goku\\goku super 3d.blend1", "goku"));
	//Model::addModel(Model::createSkybox("data\\skyboxes\\ame_siege\\", { "siege_ft.tga", "siege_bk.tga", "siege_lf.tga", "siege_rt.tga", "siege_up.tga", "siege_dn.tga" }, "siege"));
	//Model::addModel(Model::createSkybox("data\\skyboxes\\elbrus\\", { "elbrus_front.tga", "elbrus_back.tga", "elbrus_right.tga", "elbrus_left.tga", "elbrus_top.tga", "elbrus_top.tga" }, "siege"));
	Model::addModel(Model::createSkybox("data\\skyboxes\\jajlands1\\", { "jajlands1_ft.jpg", "jajlands1_bk.jpg", "jajlands1_lf.jpg", "jajlands1_rt.jpg", "jajlands1_up.jpg", "jajlands1_dn.jpg" }, "jajlands1"));
	Model::FinalizeVBO();
	
	// Create GameObjects
	GameObject* sbo = new Skybox();
	sbo->model = Model::getModel("jajlands1");
	gameObjects.push_back(sbo);

	for (int i = 0; i < 6; ++i) {
		// Houses
		GameObject* go = new House();
		go->position.x = -80.0f + i * 30.0f;
		go->position.y = -9.8f;
		go->position.z = -8;
		gameObjects.push_back(go);

		// Cars
		go = new Car();
		go->position.x = -65.0f + i * 30.0f;
		go->position.y = -9.8f;
		go->position.z = -8;
		gameObjects.push_back(go);
	}

	// Wolf
	GameObject* wolf = new Wolf();
	wolf->position.y = -9.8f;
	gameObjects.push_back(wolf);

	// Tree
	GameObject* tree = new GameObject();
	tree->model = Model::getModel("tree");
	tree->position.z = 15;
	tree->position.y = -9.8f;
	tree->rotation.x = 270.0f;
	gameObjects.push_back(tree);

	// Building
	GameObject* building = new GameObject();
	building->model = Model::getModel("building");
	building->position.z = 60.f;
	building->position.y = -9.8f;
	building->rotation.x = 270.0f;
	building->scale *= 10.0f;
	building->rotation.z = 180.0f;
	gameObjects.push_back(building);

	// Box
	GameObject* box = new GameObject();
	box->model = Model::getModel("woodenbox");
	box->scale *= 0.6f;
	box->position.z = 15.0f;
	box->position.y = -9.2f;
	gameObjects.push_back(box);

	// Colored box
	GameObject* colorBox = new GameObject();
	colorBox->model = Model::getModel("colorbox");
	colorBox->model->setTexture(Texture::getTexture("box"));
	colorBox->position.x = -20.0f;
	colorBox->position.y = -4.0f;
	colorBox->position.z = 20.0f;
	colorBox->scale.y *= 6.0f;
	colorBox->scale.z *= 3;
	colorBox->setTransparent(true);
	gameObjects.push_back(colorBox);

	// Ground
	GameObject* ground = new GameObject();
	ground->model = Model::getModel("ground");
	ground->scale = vec3(100, 0, 100);
	ground->position = vec3(0, -10, 0);
	gameObjects.push_back(ground);

	// Player
	/*GameObject* player = new Player();
	gameObjects.push_back(player);*/

	// GUI
	gui = new GUI();
	gui->setOrtho2D(1280, 720);

	// Sun
	sun = new Sun();
	gameObjects.push_back(sun);

	// Roads
	GameObject* roads = new GameObject();
	roads->model = Model::getModel("roads");
	roads->position.x = -20;
	roads->position.y = -9.8f;
	roads->position.z = 20;
	roads->rotation.x = 270;
	roads->rotation.z = 90;
	roads->scale.x *= 20;
	roads->scale.y *= 100;
	gameObjects.push_back(roads);

	// Organize list (transparent objects last)
	int len = (int) gameObjects.size();
	for (int i = 0; i < len; ++i) {
		if (gameObjects[i]->isTransparent()) {
			GameObject* tr = gameObjects[i];
			gameObjects.erase(gameObjects.begin() + i);
			gameObjects.push_back(tr);
		}
	}

	television = new Television();
	television->create();
}

void initScene() {	
	TestSetup3D();
}

void renderScene3D() {
	// AssImp
	Model::BindModelsVAO();

	// Get Shader attributes' locations
	int projectionMatrixLocation = Shader::shader->getUniformLocation("projectionMatrix");
	int viewMatrixLocation = Shader::shader->getUniformLocation("viewMatrix");
	// Render to Television
	television->startDrawing();
	// Draw objects
	for (int i = 0; i < (int)gameObjects.size(); ++i) {
		gameObjects[i]->draw();
	}

	// Go back
	television->stopDrawing();


	Camera* camera = Camera::mainCamera;
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &camera->getProjectionMatrix()[0][0]);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &camera->getViewMatrix()[0][0]);


	// Update objects
	int len = gameObjects.size();
	for (int i = 0; i < len; ++i) {
		gameObjects[i]->update();
	}

	// Draw objects
	for (int i = 0; i < len; ++i) {
		gameObjects[i]->draw();
	}

	// Draw Cinema
	television->draw();

}

void renderScene(void) {
	renderScene3D();
}