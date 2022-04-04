#pragma once
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 norm;
};

struct VertexClip
{
	glm::vec4 pos;
	glm::vec3 norm;
};

