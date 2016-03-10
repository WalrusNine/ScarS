#include "Car.h"

Car::Car() {
	position = vec3(0.0f);
	rotation = vec3(0.0f);
	scale = vec3(3.0f);

	model = Model::getModel("car");
}