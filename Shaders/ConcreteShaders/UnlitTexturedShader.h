﻿#pragma once
#include "../MyMain/shape.h"
#include "../utils/Texture.h"
#include "../Shader.h"

class UnlitTexturedShader : public Shader
{
	const Texture* texture;

	glm::mat4 mvp;

	glm::vec2 v0TC;
	glm::vec2 v1TC;
	glm::vec2 v2TC;


public:

	void preparePerObjectVertexShaderData(const MVPMat& trans) override
	{
		mvp = trans.proj * trans.view * trans.model;
	}

	void preparePerMeshData(std::unique_ptr<Mesh> const& mesh) override
	{
		texture = mesh->getTexturePtr();
	}

	TriangleClipPos
	computeVertexShader(const Vertex& v0, const Vertex& v1, const Vertex& v2) override
	{
		glm::vec4 clip_a = mvp * glm::vec4(v0.pos, 1.0f);
		glm::vec4 clip_b = mvp * glm::vec4(v1.pos, 1.0f);
		glm::vec4 clip_c = mvp * glm::vec4(v2.pos, 1.0f);

		v0TC = v0.texCoord;
		v1TC = v1.texCoord;
		v2TC = v2.texCoord;

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}

	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) override
	{
		const glm::vec2 frag_tex_coord = v0TC * w0 + v1TC * w1 + v2TC * w2;

		return texture->sampleTexture(frag_tex_coord);
	}

	bool supportsBackFaceCulling() override
	{
		return true;
	}

	std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                              std::pair<float, TriangleSide> c) override { return nullptr; }
};
