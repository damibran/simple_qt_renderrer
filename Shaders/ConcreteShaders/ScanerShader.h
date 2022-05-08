#pragma once
#include<glm/glm.hpp>
#include "../MyMain//Shape.h"
#include "../Shader.h"

class ScanerShader : public Shader
{
protected:
	struct vrtx
	{
		glm::vec3 view_pos;
	};

	vrtx a;
	vrtx b;
	vrtx c;

	glm::mat4 mvp;
	glm::mat4 mv;

	glm::vec2 v0TC;
	glm::vec2 v1TC;
	glm::vec2 v2TC;

	const Texture* texture = nullptr;

	float ambient = 0.6f;

	const float r = 30;

public:

	void preparePerObjectVertexShaderData(const MVPMat& trans) override
	{
		mvp = trans.proj * trans.view * trans.model;
		mv = trans.view * trans.model;
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

		//calculating view positions
		a.view_pos = glm::vec3(mv * glm::vec4(v0.pos, 1.0f));
		b.view_pos = glm::vec3(mv * glm::vec4(v1.pos, 1.0f));
		c.view_pos = glm::vec3(mv * glm::vec4(v2.pos, 1.0f));

		v0TC = v0.texCoord;
		v1TC = v1.texCoord;
		v2TC = v2.texCoord;

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}

	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) override
	{
		glm::vec3 view_pixel_pos = w0 * a.view_pos + w1 * b.view_pos + w2 * c.view_pos;
		const glm::vec2 frag_tex_coord = v0TC * w0 + v1TC * w1 + v2TC * w2;
		float l = length(view_pixel_pos);
		if (l < r)
		{
			l = glm::clamp(l, r - 6, r);

			glm::vec3 color = texture->sampleTexture(frag_tex_coord);

			float k = glm::clamp(1.f - (1.f / (1 * l - (r - 7))), 0.f, 1.f);

			glm::vec3 t = k * (glm::vec3(0, 0, 1) - color) + color;

			return t;
		}
		else
		{
			return texture->sampleTexture(frag_tex_coord) * ambient / 3.0f;
		}
	}

	bool supportsBackFaceCulling() override
	{
		return true;
	}

	std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
		std::pair<float, TriangleSide> c) override
	{
		return nullptr;
	}
};
