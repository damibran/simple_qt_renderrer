#pragma once
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 texCoord;
};

struct VertexView
{
	glm::vec4 pos;
	glm::vec3 norm;
};

