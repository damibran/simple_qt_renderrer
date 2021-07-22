#pragma once
#include<glm/glm.hpp>

struct MVP_mat
{
	glm::mat4 model = glm::mat4(1);
	glm::mat4 view;
	glm::mat4 proj;
};
