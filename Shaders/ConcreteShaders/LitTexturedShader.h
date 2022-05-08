#pragma once
#include "../MyMain//Shape.h"
#include "../Shader.h"

class LitTexturedShader : public Shader
{
protected:
	struct vrtx
	{
		glm::vec3 view_pos;
		glm::vec3 view_norm;
	};

	vrtx a;
	vrtx b;
	vrtx c;

	glm::mat4 mvp;
	glm::mat4 mv;
	glm::mat3 timv;
	glm::mat4 view;

	glm::vec2 v0TC;
	glm::vec2 v1TC;
	glm::vec2 v2TC;

	const Texture* texture=nullptr;

	glm::vec3 view_light_pos;

	float ambient = 0.6f;
	float diffStrength = 2.0f;
	float specStrength = 1.0f;

	std::unique_ptr<Transform>& light_obj;

public:
	LitTexturedShader(std::unique_ptr<Transform>& lo) :light_obj(lo)
	{
	}

	LitTexturedShader() = delete;

	void preparePerObjectVertexShaderData(const MVPMat& trans) override
	{
		view = trans.view;
		mvp = trans.proj * trans.view * trans.model;
		mv = trans.view * trans.model;
		timv = glm::mat3(glm::transpose(glm::inverse(trans.view * trans.model)));
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

		//calculation normals
		a.view_norm = timv * v0.norm;
		b.view_norm = timv * v1.norm;
		c.view_norm = timv * v2.norm;

		view_light_pos = view * glm::vec4(light_obj->getPos(), 1.0f);

		v0TC = v0.texCoord;
		v1TC = v1.texCoord;
		v2TC = v2.texCoord;

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}

	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) override
	{
		glm::vec3 view_pixel_pos = w0 * a.view_pos + w1 * b.view_pos + w2 * c.view_pos;
		const glm::vec2 frag_tex_coord = v0TC * w0 + v1TC * w1 + v2TC * w2;

		glm::vec3 norm_pixel = glm::normalize(w0 * a.view_norm + w1 * b.view_norm + w2 * c.view_norm);
		glm::vec3 lightDir = glm::normalize(view_light_pos - view_pixel_pos);

		glm::vec3 viewDir = normalize(-view_pixel_pos);
		glm::vec3 reflectDir = glm::normalize(glm::reflect(-lightDir, norm_pixel));

		float diff = glm::clamp(std::fmaxf(glm::dot(norm_pixel, lightDir), 0.0f), 0.0f, 1.0f);
		float spec = glm::clamp(std::pow(std::fmaxf(glm::dot(viewDir, reflectDir), 0.0f), 32), 0.0, 1.0);

		glm::vec3 color = texture->sampleTexture(frag_tex_coord) * (ambient / 3 + diff * diffStrength / 3 + spec * specStrength / 3);
		return color;
	}

	bool supportsBackFaceCulling() override
	{
		return true;
	}

	std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                              std::pair<float, TriangleSide> c) override
	{
		std::unique_ptr<LitTexturedShader> res = std::make_unique<LitTexturedShader>(
			this->light_obj);
		res->view_light_pos = view_light_pos;
		res->a.view_pos = lerpViewPosAlongSide(a.first, a.second);
		res->a.view_norm = lerpViewNormAlongSide(a.first, a.second);

		res->b.view_pos = lerpViewPosAlongSide(b.first, b.second);
		res->b.view_norm = lerpViewNormAlongSide(b.first, b.second);

		res->c.view_pos = lerpViewPosAlongSide(c.first, c.second);
		res->c.view_norm = lerpViewNormAlongSide(c.first, c.second);

		return res;
	}

protected:
	glm::vec3 lerpViewPosAlongSide(float t, TriangleSide side) const
	{
		if (side == TriangleSide::AB)
		{
			return this->a.view_pos + t * (this->b.view_pos - this->a.view_pos);
		}
		if (side == TriangleSide::BC)
		{
			return this->b.view_pos + t * (this->c.view_pos - this->b.view_pos);
		}
		return this->c.view_pos + t * (this->a.view_pos - this->c.view_pos);
	}

	glm::vec3 lerpViewNormAlongSide(float t, TriangleSide side) const
	{
		if (side == TriangleSide::AB)
		{
			return this->a.view_norm + t * (this->b.view_norm - this->a.view_norm);
		}
		if (side == TriangleSide::BC)
		{
			return this->b.view_norm + t * (this->c.view_norm - this->b.view_norm);
		}
		return this->c.view_norm + t * (this->a.view_norm - this->c.view_norm);
	}
};
