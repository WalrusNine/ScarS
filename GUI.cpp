#include "GUI.h"
#include "Screen.h"

GUI::GUI() : GameObject("gui") {
	ftf = new FreeTypeFont();
	bool rsc = ftf->LoadFont("data/fonts/arial.ttf", 32, 1024);

	ftf->SetShaderProgram(Shader::shader);

	width = Screen::width;
	height = Screen::height;
}

GUI::~GUI() {
	delete ftf;
}

void GUI::SetOrtho2D(int w, int h) {
	orthoMatrix = glm::ortho(0.0f, float(w), 0.0f, float(h));
}
void GUI::SetOrtho2D() {
	orthoMatrix = glm::ortho(0.0f, float(width), 0.0f, float(height));
}


glm::mat4 GUI::GetOrthoMatrix() {
	return orthoMatrix;
}

void GUI::update() {
	if (InputController::getInputState(true, GLFW_KEY_P) == INPUT_RELEASED) {
		enabled = !enabled;
	}
}

void GUI::draw(bool shadow) {
	if (!shadow && enabled) {
		Shader::shader->bind("font");
		glDisable(GL_DEPTH_TEST);
		Shader::shader->SetUniform("projectionMatrix", GetOrthoMatrix());

		Shader::shader->SetUniform("vColor", glm::vec4(1.f, 1.f, 0.f, 1.0f));
		ftf->PrintFormatted(20, height - 30, 24, "%d", (int) (1 / FPSController::getDeltaTime()));
		glEnable(GL_DEPTH_TEST);
		Shader::shader->bind("main");
	}
}
