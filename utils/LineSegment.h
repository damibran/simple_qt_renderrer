#pragma once
#include <glm/glm.hpp>
struct LineSeg
{
	LineSeg() = default;
	LineSeg(const glm::vec3 _a, const glm::vec3 _b) :a(_a), b(_b) {};
	glm::vec3 a;
	glm::vec3 b;
};