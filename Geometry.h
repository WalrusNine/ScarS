#pragma once
#include <vector>
//Include GLEW  
#include <GL/glew.h>  
#include <glm/glm.hpp>

enum GeometryType
{
	GEOMETRY_NONE,
	GEOMETRY_PLANE,
	GEOMETRY_CUBE,
	GEOMETRY_PYRAMID,
	GEOMETRY_SPHERE
};

class Geometry
{
public:
	
	static std::vector<glm::vec3> getGeometryVertexPosition(std::string name);
	static std::vector<glm::vec2> getGeometryVertexUV(std::string name);

private:
};