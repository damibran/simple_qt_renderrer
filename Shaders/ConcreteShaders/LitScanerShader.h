#pragma once
#include<glm/glm.hpp>
#include "../MyMain//Shape.h"
#include "../Shader.h"

class LitScanerShader : public Shader
{
protected:
	struct vrtx
	{
		glm::vec3 view_pos;
		glm::vec3 view_norm;
		glm::vec2 TC;
	};

	vrtx a;
	vrtx b;
	vrtx c;

	glm::mat4 view;
	glm::mat4 mvp;
	glm::mat4 mv;
	glm::mat3 timv;

	glm::vec3 view_light_pos;

	const Texture* texture = nullptr;

	float ambient = 0.6f;
	float diffStrength = 2.0f;
	float specStrength = 1.0f;

	const float r = 30;

	std::unique_ptr<Transform>& light_obj;


public:
	LitScanerShader(std::unique_ptr<Transform>& lo) : light_obj(lo)
	{
	}

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

		a.TC = v0.texCoord;
		b.TC = v1.texCoord;
		c.TC = v2.texCoord;

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}

	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) override
	{
		glm::vec3 view_pixel_pos = w0 * a.view_pos + w1 * b.view_pos + w2 * c.view_pos;
		const glm::vec2 frag_tex_coord = a.TC * w0 + b.TC * w1 + c.TC * w2;

		float l = length(view_pixel_pos);

		if (l < r)
		{
			l = glm::clamp(l, r - 6, r);

			glm::vec3 color = PhongColor(w0, w1, w2);

			float k = glm::clamp(1.f - (1.f / (1 * l - (r - 7))), 0.f, 1.f);

			glm::vec3 t = k * (glm::vec3(0, 0, 1) - color) + color;

			return t;
		}
		else
		{
			return texture->sampleTexture(frag_tex_coord) * ambient / 3.0f;
		}
	}

	glm::vec3 PhongColor(float w0, float w1, float w2)
	{
		glm::vec3 view_pixel_pos = w0 * a.view_pos + w1 * b.view_pos + w2 * c.view_pos;
		const glm::vec2 frag_tex_coord = a.TC * w0 + b.TC * w1 + c.TC * w2;

		glm::vec3 norm_pixel = glm::normalize(w0 * a.view_norm + w1 * b.view_norm + w2 * c.view_norm);
		glm::vec3 lightDir = glm::normalize(view_light_pos - view_pixel_pos);

		glm::vec3 viewDir = normalize(-view_pixel_pos);
		glm::vec3 reflectDir = glm::normalize(glm::reflect(-lightDir, norm_pixel));

		float diff = glm::clamp(std::fmaxf(glm::dot(norm_pixel, lightDir), 0.0f), 0.0f, 1.0f);
		float spec = glm::clamp(std::pow(std::fmaxf(glm::dot(viewDir, reflectDir), 0.0f), 32), 0.0, 1.0);

		glm::vec3 color = texture->sampleTexture(frag_tex_coord) * (ambient / 3 + diff * diffStrength / 3 + spec *
			specStrength / 3);

		return color;
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
