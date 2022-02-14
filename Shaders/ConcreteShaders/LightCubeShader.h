#pragma once
#include "../shapes/shape.h"
#include "../Shader.h"

class LightCubeShader : public Shader
{
private:

	glm::vec3 objColor = glm::vec3(255, 255, 255);

public:

	LightCubeShader() {};

	triangleClipPos computeVertexShader(const MVP_mat& trans, const vertex& v0, const vertex& v1, const vertex& v2) override
	{
		glm::vec4 clip_a = trans.proj * trans.view * trans.model * glm::vec4(v0.pos, 1.0f);
		glm::vec4 clip_b = trans.proj * trans.view * trans.model * glm::vec4(v1.pos, 1.0f);
		glm::vec4 clip_c = trans.proj * trans.view * trans.model * glm::vec4(v2.pos, 1.0f);

		return triangleClipPos(clip_a, clip_b, clip_c);
	}
	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2)override
	{
		glm::vec3 color = objColor;

		return color;
	}
};