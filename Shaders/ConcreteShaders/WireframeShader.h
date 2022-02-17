#pragma once
#include "../shapes/shape.h"
#include "../Shader.h"

class WireFrameShader : public Shader
{
private:
	struct vrtx
	{
		glm::vec3 view_pos;
	};

	vrtx a;
	vrtx b;
	vrtx c;

	float line_width = 0.0001;

public:
	WireFrameShader(float lw) :line_width(lw) {}
	triangleClipPos computeVertexShader(const MVP_mat& trans, const vertex& v0, const vertex& v1, const vertex& v2) override
	{
		glm::vec4 clip_a = trans.proj * trans.view * trans.model * glm::vec4(v0.pos, 1.0f);
		glm::vec4 clip_b = trans.proj * trans.view * trans.model * glm::vec4(v1.pos, 1.0f);
		glm::vec4 clip_c = trans.proj * trans.view * trans.model * glm::vec4(v2.pos, 1.0f);

		//calculating view positions
		a.view_pos = glm::vec3(trans.view * trans.model * glm::vec4(v0.pos, 1.0f));
		b.view_pos = glm::vec3(trans.view * trans.model * glm::vec4(v1.pos, 1.0f));
		c.view_pos = glm::vec3(trans.view * trans.model * glm::vec4(v2.pos, 1.0f));

		return triangleClipPos(clip_a, clip_b, clip_c);
	}
	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2)override
	{
		glm::vec3 view_pixel_pos = w0 * a.view_pos + w1 * b.view_pos + w2 * c.view_pos;

		float z = -view_pixel_pos.z;

		if (w0 < line_width * z || w1 < line_width * z || w2 < line_width * z)
			return glm::vec3(0, 0, 0);
		else
			return glm::vec3(300, 300, 300);

	}
};