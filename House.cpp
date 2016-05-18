#include "House.h"

House::House() : GameObject("House") {
	position = vec3(0.0f);
	rotation = vec3(0.0f);
	scale = vec3(3.0f);

	model = Model::getModel("house");
}


House::~House() {

}
