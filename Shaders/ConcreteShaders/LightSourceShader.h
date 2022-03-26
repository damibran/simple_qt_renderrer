#pragma once
#include "../MyMain/shape.h"
#include "../Shader.h"

class LightSourceShader : public Shader
{

	glm::vec4 objColor = glm::vec4(1, 1, 1,1);

public:

	LightSourceShader() {}

	TriangleClipPos computeVertexShader(const MVPMat& trans, const Vertex& v0, const Vertex& v1, const Vertex& v2) override
	{
		glm::vec4 clip_a = trans.proj * trans.view * trans.model * glm::vec4(v0.pos, 1.0f);
		glm::vec4 clip_b = trans.proj * trans.view * trans.model * glm::vec4(v1.pos, 1.0f);
		glm::vec4 clip_c = trans.proj * trans.view * trans.model * glm::vec4(v2.pos, 1.0f);

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}
	glm::vec4 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2)override
	{
		glm::vec4 color = objColor;

		return color;
	}

	bool supportsBackFaceCulling() override
	{
		return true;
	}
};