#pragma once
#include "../MyMain/shape.h"
#include "../utils/Texture.h"
#include "../Shader.h"

class UnlitTexturedShader : public Shader
{
	struct vrtx
	{
		glm::vec2 TC;
	};

	vrtx a;
	vrtx b;
	vrtx c;

	const Texture* texture;

	glm::mat4 mvp;


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

		a.TC = v0.texCoord;
		b.TC = v1.texCoord;
		c.TC = v2.texCoord;

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}

	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) override
	{
		const glm::vec2 frag_tex_coord = a.TC * w0 + b.TC * w1 + c.TC * w2;

		return texture->sampleTexture(frag_tex_coord);
	}

	bool supportsBackFaceCulling() override
	{
		return true;
	}

	std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                              std::pair<float, TriangleSide> c) override
	{
		std::unique_ptr<UnlitTexturedShader> res = std::make_unique<UnlitTexturedShader>();

		res->texture = texture;
		res->mvp=mvp;

		res->a.TC = lerpTexCoordAlongSide(a.first, a.second);
		res->a.TC = lerpTexCoordAlongSide(a.first, a.second);

		res->b.TC = lerpTexCoordAlongSide(b.first, b.second);
		res->b.TC = lerpTexCoordAlongSide(b.first, b.second);

		res->c.TC = lerpTexCoordAlongSide(c.first, c.second);
		res->c.TC = lerpTexCoordAlongSide(c.first, c.second);

		return res;
	}

	glm::vec2 lerpTexCoordAlongSide(float t, TriangleSide side) const
	{
		if (side == TriangleSide::AB)
		{
			return this->a.TC + t * (this->b.TC - this->a.TC);
		}
		if (side == TriangleSide::BC)
		{
			return this->b.TC + t * (this->c.TC - this->b.TC);
		}
		return this->c.TC + t * (this->a.TC - this->c.TC);
	}
};
