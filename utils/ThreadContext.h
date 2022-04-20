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
		for (int i = 0; i < 2; ++i)
		{
			color_buffer[i].resize(w * h);
			z_buffer_[i].resize(w * h);
			for (size_t j = 0; j < w * h; ++j)
				z_buffer_[i][j] = FLT_MAX;
		}
	}

	float getZBufferAt(int indx) const
	{
		return z_buffer_[cur_buffer][indx];
	}

	void setZBufferAt(int indx, float f)
	{
		z_buffer_[cur_buffer][indx] = f;
	}

	void setColorBufferAt(int indx, const glm::vec3& c)
	{
		color_buffer[cur_buffer][indx]=c;
	}

	void setColorBufferAt(size_t x, size_t y, const glm::vec3& c)
	{
		color_buffer[cur_buffer][y * w_ + x]=c;
	}

	uint cur_buffer=0;
	uint w_, h_;
	std::array<std::vector<glm::vec3>, 2> color_buffer;
	std::array<std::vector<float>, 2> z_buffer_;
	std::vector<std::unique_ptr<Shader>> shaders_;
};
