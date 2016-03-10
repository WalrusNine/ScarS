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
#include "FreeTypeFont.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

std::vector<GameObject*> gameObjects;
GUI* gui;
//FreeTypeFont* ftf;

void TestSetup3D() {
	// Create Camera
	/*Camera::mainCamera = Camera::createCamera();
	gameObjects.push_back((GameObject*)Camera::mainCamera);*/
	SkyCamera::mainCamera = SkyCamera::createCamera();
	gameObjects.push_back((GameObject*)SkyCamera::mainCamera);

	// Create Textures
	Texture::addTexture(Texture::createTexture("data\\textures\\golddiag.jpg", TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, "golddiag", true));
	Texture::addTexture(Texture::createTexture("data\\textures\\snow.jpg", TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, "snow", true));

	// Create Models
	Model::addModel(Model::createModel("data\\models\\FireGTO\\FireGTO.blend", "car"));
	Model::addModel(Model::createModel("data\\models\\house\\house.3ds", "house"));
	//Model::addModel(Model::createModel("data\\models\\Wolf\\Wolf.obj", "wolf"));
	Model::addModel(Model::createModel("data\\models\\tree\\Tree1.3ds", "tree"));
	Model::addModel(Model::createModel("data\\models\\building\\Small_Building_1.blend", "building"));
	Model::addModel(Model::createGeometry(GEOMETRY_PLANE, Texture::getTexture("snow"), "ground"));
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
		gameObjects.push_back(go);

		// Cars
		go = new Car();
		go->position.x = -65.0f + i * 30.0f;
		go->position.y = -9.8f;
		gameObjects.push_back(go);
	}

	// Wolf
	/*GameObject* wolf = new Wolf();
	wolf->position.y = -9.8f;
	gameObjects.push_back(wolf);*/

	GameObject* tree = new GameObject();
	tree->model = Model::getModel("tree");
	tree->position.z = 15;
	tree->position.y = -9.8f;
	tree->rotation.x = 270.0f;
	gameObjects.push_back(tree);

	GameObject* building = new GameObject();
	building->model = Model::getModel("building");
	building->position.z = -40.f;
	building->position.y = -9.8f;
	building->rotation.x = 270.0f;
	building->scale *= 10.0f;
	gameObjects.push_back(building);

	// Ground
	GameObject* ground = new GameObject();
	ground->model = Model::getModel("ground");
	ground->scale = vec3(100, 0, 100);
	ground->position = vec3(0, -10, 0);
	gameObjects.push_back(ground);

	// Player
	//GameObject* player = new Player();
	//gameObjects.push_back(player);

	// GUI
	gui = new GUI();
	gui->setOrtho2D(1280, 720);

	//ftf = new FreeTypeFont();
	//ftf->loadSystemFont("comic.ttf", 32);

	// Sun

}

void initScene()
{	
	TestSetup3D();
}

float sunAngle = 45.0f;

void renderScene3D() {
	// Get Shader attributes' locations
	int projectionMatrixLocation = Shader::shader->getUniformLocation("projectionMatrix");
	int viewMatrixLocation = Shader::shader->getUniformLocation("viewMatrix");

	// MVP
	Camera* camera = Camera::mainCamera;
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &camera->getProjectionMatrix()[0][0]);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &camera->getViewMatrix()[0][0]);
	
	// Sampler
	int sampler = Shader::shader->getUniformLocation("gSampler");
	glUniform1i(sampler, 0);
	
	// Light
	float sine = sin(sunAngle*3.1415 / 180.0);
	glm::vec3 sunPos(cos(sunAngle*3.1415 / 180.0) * 70, sin(sunAngle*3.1415 / 180.0) * 70, 0.0);
	//sunPos = vec3 (0, -100, 0);
	int loc = 0;
	loc = Shader::shader->getUniformLocation("sunlight.color");
	glUniform3fv(loc, 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
	loc = Shader::shader->getUniformLocation("sunlight.ambient_intensity");
	float ambientIntensity = 1.0f;
	glUniform1fv(loc, 1, &ambientIntensity);
	loc = Shader::shader->getUniformLocation("sunlight.direction");
	vec3 norm = -glm::normalize(sunPos);
	glUniform3fv(loc, 1, &norm[0]);

	// AssImp
	Model::BindModelsVAO();

	// Update objects
	int len = gameObjects.size();
	for (int i = 0; i < len; ++i) {
		gameObjects[i]->update();
	}
	// Draw objects
	for (int i = 0; i < len; ++i) {
		gameObjects[i]->draw();
	}

	// Draw text
	//glDisable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS);
	//glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &mat4(1.0f)[0][0]);
	//glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &gui->getOrthoMatrix()[0][0]);

	//char buf[256];
	//sprintf_s(buf, "Font Size: %d\nPress UP and DOWN arrow key to change\n\nTotal Torus Faces: %d", 24, 5);

	//ftf->print(buf, 20, 720 - 10 - 24, 24);
	//ftf->print("www.mbsoftworks.sk", 20, 20, 24);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	// Draw Sun
	/*VertexBufferObject* vbo = new VertexBufferObject();
	mat4 sun = translate(mat4(1.0), sunPos);
	Shader::shader->setUniform("modelMatrix", sun);
	Shader::shader->setUniform("normalMatrix", glm::transpose(glm::inverse(sun)));
	ambientIntensity = 0.8f;  glUniform1fv(loc, 1, &ambientIntensity);*/

	if (InputController::getInputState(true, GLFW_KEY_1) == INPUT_HELD) {
		sunAngle -= (45.0f) * FPSController::getDeltaTime();
	}
	else if (InputController::getInputState(true, GLFW_KEY_2) == INPUT_HELD) {
		sunAngle += (45.0f) * FPSController::getDeltaTime();
	}
}

void renderScene(void)
{
	renderScene3D();
}