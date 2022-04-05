#pragma once
#include "../MyMain//Shape.h"
#include "../Shader.h"

class OnePointSourceLitShaderWithWireframe final : public Shader
{
private:
	struct vrtx
	{
		glm::vec3 view_pos;
		glm::vec3 view_norm;
	};

	vrtx a;
	vrtx b;
	vrtx c;

	glm::vec3 view_light_pos;

	float ambient = 0.1f;
	float diffStrength = 0.5f;
	float specStrength = 0.8f;

	glm::vec3 objColor = glm::vec3(255, 255, 84);

	std::unique_ptr<Transform>& light_obj;

public:
	OnePointSourceLitShaderWithWireframe(std::unique_ptr<Transform>& lo) : light_obj(lo)
	{
	}

	OnePointSourceLitShaderWithWireframe() = delete;

	TriangleClipPos
	computeVertexShader(const MVPMat& trans, const Vertex& v0, const Vertex& v1, const Vertex& v2) override
	{
		glm::vec4 clip_a = trans.proj * trans.view * trans.model * glm::vec4(v0.pos, 1.0f);
		glm::vec4 clip_b = trans.proj * trans.view * trans.model * glm::vec4(v1.pos, 1.0f);
		glm::vec4 clip_c = trans.proj * trans.view * trans.model * glm::vec4(v2.pos, 1.0f);

		//calculating view positions
		a.view_pos = glm::vec3(trans.view * trans.model * glm::vec4(v0.pos, 1.0f));
		b.view_pos = glm::vec3(trans.view * trans.model * glm::vec4(v1.pos, 1.0f));
		c.view_pos = glm::vec3(trans.view * trans.model * glm::vec4(v2.pos, 1.0f));

		//calculation normals
		a.view_norm = glm::mat3(glm::transpose(glm::inverse(trans.view * trans.model))) * v0.norm;
		b.view_norm = glm::mat3(glm::transpose(glm::inverse(trans.view * trans.model))) * v1.norm;
		c.view_norm = glm::mat3(glm::transpose(glm::inverse(trans.view * trans.model))) * v2.norm;

		view_light_pos = trans.view * glm::vec4(light_obj->getPos(), 1.0f);

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}

	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) override
	{
		glm::vec3 view_pixel_pos = w0 * a.view_pos + w1 * b.view_pos + w2 * c.view_pos;

		float z = -view_pixel_pos.z;
		const float line_width = 0.0005;
		if (w0 < line_width * z || w1 < line_width * z || w2 < line_width * z)
			return {0, 0, 0};

		glm::vec3 norm_pixel = glm::normalize(w0 * a.view_norm + w1 * b.view_norm + w2 * c.view_norm);
		glm::vec3 lightDir = glm::normalize(view_light_pos - view_pixel_pos);

		glm::vec3 viewDir = normalize(-view_pixel_pos);
		glm::vec3 reflectDir = glm::normalize(glm::reflect(-lightDir, norm_pixel));

		float diff = glm::clamp(std::fmaxf(glm::dot(norm_pixel, lightDir), 0.0f), 0.0f, 1.0f);
		float spec = glm::clamp(std::pow(std::fmaxf(glm::dot(viewDir, reflectDir), 0.0f), 32), 0.0, 1.0);

		glm::vec3 color = objColor * (ambient / 3 + diff * diffStrength / 3 + spec * specStrength / 3);
		return color;
	}

	bool supportsBackFaceCulling() override
	{
		return false;
	}

	std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                              std::pair<float, TriangleSide> c) override
	{
		std::unique_ptr<OnePointSourceLitShaderWithWireframe> res = std::make_unique<
			OnePointSourceLitShaderWithWireframe>(this->light_obj);

		res->view_light_pos=view_light_pos;

		res->a.view_pos = lerpViewPosAlongSide(a.first, a.second);
		res->a.view_norm = lerpViewNormAlongSide(a.first, a.second);

		res->b.view_pos = lerpViewPosAlongSide(b.first, b.second);
		res->b.view_norm = lerpViewNormAlongSide(b.first, b.second);

		res->c.view_pos = lerpViewPosAlongSide(c.first, c.second);
		res->c.view_norm = lerpViewNormAlongSide(c.first, c.second);

		return res;
	}

private:
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
