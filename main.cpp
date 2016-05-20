//Include GLEW  
#include <GL/glew.h>  

//Include GLFW  
#include <GLFW/glfw3.h>

//Include the standard C++ headers  
#include <stdio.h>
#include <stdlib.h>

#include "GameObject.h"
#include "FPSController.h"
#include "InputController.h"
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

//Declare a window object  
GLFWwindow* window;

//Define an error callback  
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

bool initialize(int argc, char* argv[]) {
	

	//Set the error callback  
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW  
	if (!glfwInit())
	{
		return false;
	}

	//Set the GLFW window creation hints - these are optional  
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version  
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version  
	//glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing  
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create a window and create its OpenGL context  
	window = glfwCreateWindow(1280, 720, "Test Window", NULL, NULL);

	//If the window couldn't be created  
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return false;
	}

	//This function makes the context of the specified window current on the calling thread.   
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, 0);

	//Sets the key callback  
	//glfwSetKeyCallback(window, key_callback);

	//Initialize GLEW  
	GLenum err = glewInit();

	//If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
		return false;
	}
	
	// Load shaders
	Shader::shader = new Shader();
	Shader::shader->init();
	Shader::shader->bind();

	//Set a background color
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	return true;
}

void mainLoop() {
	//Get controllers
	FPSController* fpsController = new FPSController();
	InputController* inputController = new InputController(*window);
	Scene* scene = new Scene();

	//Main Loop
	do
	{
		//Update FPSController
		fpsController->update();
		//Get frame delta time
		float deltaTime = fpsController->getDeltaTime();

		//Update InputController
		inputController->update();

		//Clear color buffer  
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render
		scene->render();

		//Swap buffers  
		glfwSwapBuffers(window);

		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();

	} //Check if the ESC key had been pressed or if the window had been closed  
	while (!glfwWindowShouldClose(window) && 
		inputController->getInputState(true, GLFW_KEY_ESCAPE) != INPUT_RELEASED);

	delete fpsController;
	delete inputController;
	delete scene;
	delete Shader::shader;
}

void finalize() {
	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow(window);
	//Finalize and clean up GLFW
	glfwTerminate();
}

int main(int argc, char* argv[])
{
	if (!initialize(argc, argv)) {
		return EXIT_FAILURE;
	}

	mainLoop();

	finalize();

	exit(EXIT_SUCCESS);
}