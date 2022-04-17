#pragma once
#include "../MyMain//Shape.h"
#include "../ConcreteShaders/OnePointSourceLitShader.h"

class OnePointSourceLitShaderWithWireframe final : public OnePointSourceLitShader
{
public:
	OnePointSourceLitShaderWithWireframe(std::unique_ptr<Transform>& lo,
	                                     glm::vec3 color = glm::vec3(255, 255, 84)): OnePointSourceLitShader(lo, color)
	{
	}

	OnePointSourceLitShaderWithWireframe() = delete;

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

	std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                              std::pair<float, TriangleSide> c)const override
	{
		std::unique_ptr<OnePointSourceLitShaderWithWireframe> res = std::make_unique<
			OnePointSourceLitShaderWithWireframe>(this->light_obj, this->objColor);
		res->view_light_pos = view_light_pos;

		res->a.view_pos = lerpViewPosAlongSide(a.first, a.second);
		res->a.view_norm = lerpViewNormAlongSide(a.first, a.second);

		res->b.view_pos = lerpViewPosAlongSide(b.first, b.second);
		res->b.view_norm = lerpViewNormAlongSide(b.first, b.second);

		res->c.view_pos = lerpViewPosAlongSide(c.first, c.second);
		res->c.view_norm = lerpViewNormAlongSide(c.first, c.second);
		return res;
	}
};
