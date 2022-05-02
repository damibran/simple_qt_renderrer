#pragma once
#include "../MyMain/shape.h"
#include "../Shader.h"

class LightSourceShader : public Shader
{
	glm::mat4 mvp;
	glm::vec3 objColor = glm::vec3(255);

public:
	LightSourceShader()
	{
	}

	void preparePerObjectVertexShaderData(const MVPMat& trans) override
	{
		mvp = trans.proj * trans.view * trans.model;
	}

	TriangleClipPos
	computeVertexShader(const Vertex& v0, const Vertex& v1, const Vertex& v2) override
	{
		glm::vec4 clip_a = mvp * glm::vec4(v0.pos, 1.0f);
		glm::vec4 clip_b = mvp * glm::vec4(v1.pos, 1.0f);
		glm::vec4 clip_c = mvp * glm::vec4(v2.pos, 1.0f);

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}

	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) override
	{
		return objColor;
	}

	bool supportsBackFaceCulling() override
	{
		return true;
	}

	std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                              std::pair<float, TriangleSide> c) override { return nullptr; }
};
