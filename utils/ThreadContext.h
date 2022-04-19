#pragma once
#include <memory>
#include <vector>
#include "../Shaders/Shader.h"

typedef unsigned int uint;

struct ThreadContext
{
	ThreadContext() = default;

	ThreadContext(uint w, uint h): w_(w), h_(h)
	{
		for(int i=0;i<2;++i)
		{
		color_buffer[i].resize(w * h);
		z_buffer_[i].resize(w * h);
		for (size_t j = 0; j < w * h; ++j)
			z_buffer_[i][j] = FLT_MAX;
		}
	}

	uint w_, h_;
	std::array<std::vector<glm::vec3>,2> color_buffer;
	std::array<std::vector<float>,2> z_buffer_;
	std::vector<std::unique_ptr<Shader>> shaders_;
};