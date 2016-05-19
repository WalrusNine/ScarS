#include "Geometry.h"


// Vertices

// Plane
std::vector<glm::vec3> planeVerticesPosition = {
	// Triangle 1
	{ -1.0f, 0.0f, -1.0f },	{ 1.0f, 0.0f, -1.0f }, { 1.0f, 0.f, 1.0f },
	// Triangle 2
	{ 1.0f, 0.f, 1.0f }, { -1.0f, 0.f, 1.0f }, { -1.0f, 0.f, -1.0f }
};

std::vector<glm::vec2> planeVerticesUV = {
	// Triangle 1
	{ 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f },
	// Triangle 2
	{ 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f },
};

// Cube
std::vector<glm::vec3> cubeVerticesPosition = {
	// Triangle 1
	{ -1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, -1.0f, 1.0f },
	// Triangle 2
	{ 1.0f, -1.0f, 1.0f },{ -1.0f, -1.0f, 1.0f },{ -1.0f, 1.0f, 1.0f },
	// Triangle 3
	{ 1.0f, 1.0f, -1.0f },{ -1.0f, 1.0f, -1.0f },{ -1.0f, -1.0f, -1.0f },
	// Triangle 4
	{ -1.0f, -1.0f, -1.0f },{ 1.0f, -1.0f, -1.0f },{ 1.0f, 1.0f, -1.0f },
	// Triangle 5
	{ -1.0f, 1.0f, -1.0f },{ -1.0f, 1.0f, 1.0f },{ -1.0f, -1.0f, 1.0f },
	// Triangle 6
	{ -1.0f, -1.0f, 1.0f },{ -1.0f, -1.0f, -1.0f },{ -1.0f, 1.0f, -1.0f },
	// Triangle 7
	{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f, -1.0f },{ 1.0f, -1.0f, -1.0f },
	// Triangle 8
	{ 1.0f, -1.0f, -1.0f },{ 1.0f, -1.0f, 1.0f },{ 1.0f, 1.0f, 1.0f },
	// Triangle 9
	{ -1.0f, 1.0f, -1.0f },{ 1.0f, 1.0f, -1.0f },{ 1.0f, 1.0f, 1.0f },
	// Triangle 10
	{ 1.0f, 1.0f, 1.0f },{ -1.0f, 1.0f, 1.0f },{ -1.0f, 1.0f, -1.0f },
	// Triangle 11
	{ -1.0f, -1.0f, 1.0f },{ 1.0f, -1.0f, 1.0f },{ 1.0f, -1.0f, -1.0f },
	// Triangle 12
	{ 1.0f, -1.0f, -1.0f },{ -1.0f, -1.0f, -1.0f },{ -1.0f, -1.0f, 1.0f }
};

std::vector<glm::vec2> cubeVerticesUV = {
	// Triangle 1
	{ 0.0f, 1.0f },{ 1.0f, 1.0f },{ 1.0f, 0.0f },
	// Triangle 2
	{ 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f },
	// Triangle 3
	{ 0.0f, 1.0f },{ 1.0f, 1.0f },{ 1.0f, 0.0f },
	// Triangle 4
	{ 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f },
	// Triangle 5
	{ 0.0f, 1.0f },{ 1.0f, 1.0f },{ 1.0f, 0.0f },
	// Triangle 6
	{ 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f },
	// Triangle 7
	{ 0.0f, 1.0f },{ 1.0f, 1.0f },{ 1.0f, 0.0f },
	// Triangle 8
	{ 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f },
	// Triangle 9
	{ 0.0f, 1.0f },{ 1.0f, 1.0f },{ 1.0f, 0.0f },
	// Triangle 10
	{ 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f },
	// Triangle 11
	{ 0.0f, 1.0f },{ 1.0f, 1.0f },{ 1.0f, 0.0f },
	// Triangle 12
	{ 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f },
};

std::vector<glm::vec3> Geometry::getGeometryVertexPosition(std::string name) {
	if (name == "plane")
		return planeVerticesPosition;
	else if (name == "cube")
		return cubeVerticesPosition;
}

std::vector<glm::vec2> Geometry::getGeometryVertexUV(std::string name) {
	if (name == "plane")
		return planeVerticesUV;
	else if (name == "cube")
		return cubeVerticesUV;
}
