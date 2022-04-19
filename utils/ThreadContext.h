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
		color_buffer.resize(w * h);
		z_buffer_.resize(w * h);
		for (size_t i = 0; i < w * h; ++i)
			z_buffer_[i] = FLT_MAX;
	}

	uint w_, h_;
	std::vector<glm::vec3> color_buffer;
	std::vector<float> z_buffer_;
	std::vector<std::unique_ptr<Shader>> shaders_;
};