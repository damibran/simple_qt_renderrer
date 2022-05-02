#pragma once
#include "../MyMain/Shape.h"
#include "../Shader.h"

class WireFrameShader : public Shader
{
private:
	struct vrtx
	{
		glm::vec3 view_pos;
	};

	vrtx a;
	vrtx b;
	vrtx c;

	glm::mat4 mv;
	glm::mat4 mvp;


	float line_width;

public:
	WireFrameShader(float lw=0.0001) : line_width(lw)
	{
	}

	void preparePerObjectVertexShaderData(const MVPMat& trans) override
	{
		mvp = trans.proj * trans.view * trans.model;
		mv = trans.view * trans.model ;
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

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}

	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) override
	{
		glm::vec3 view_pixel_pos = w0 * a.view_pos + w1 * b.view_pos + w2 * c.view_pos;

		float z = -view_pixel_pos.z;

		if (w0 < line_width * z || w1 < line_width * z || w2 < line_width * z)
			return glm::vec3(0);
		else
			return glm::vec3(-1); // discard 
	}

	bool supportsBackFaceCulling() override
	{
		return false;
	}

	std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                              std::pair<float, TriangleSide> c) override
	{
		std::unique_ptr<WireFrameShader> res = std::make_unique<WireFrameShader>(line_width);

		res->a.view_pos = lerpViewPosAlongSide(a.first, a.second);

		res->b.view_pos = lerpViewPosAlongSide(b.first, b.second);

		res->c.view_pos = lerpViewPosAlongSide(c.first, c.second);

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
};
