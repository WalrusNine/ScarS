#include "Screen.h"

int Screen::width;
int Screen::height;

void Screen::SetViewportSize(int w, int h) {
	width = w;
	height = h;
}
