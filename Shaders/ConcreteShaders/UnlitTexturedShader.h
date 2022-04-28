#pragma once
#include "../MyMain/shape.h"
#include "../utils/Texture.h"
#include "../Shader.h"

class UnlitTexturedShader : public Shader
{
	Texture* texture;

	glm::vec2 v0TC;
	glm::vec2 v1TC;
	glm::vec2 v2TC;


public:
	UnlitTexturedShader(Texture* text): texture(text)
	{
	}

	TriangleClipPos
	computeVertexShader(const MVPMat& trans, const Vertex& v0, const Vertex& v1, const Vertex& v2) override
	{
		glm::vec4 clip_a = trans.proj * trans.view * trans.model * glm::vec4(v0.pos, 1.0f);
		glm::vec4 clip_b = trans.proj * trans.view * trans.model * glm::vec4(v1.pos, 1.0f);
		glm::vec4 clip_c = trans.proj * trans.view * trans.model * glm::vec4(v2.pos, 1.0f);

		v0TC = v0.texCoord;
		v1TC = v1.texCoord;
		v2TC = v2.texCoord;

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}

	glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) override
	{

		const glm::vec2 frag_tex_coord = v0TC * w0 + v1TC * w1 + v2TC * w2;

		int t_x = frag_tex_coord.x*texture->width;
		int t_y = frag_tex_coord.y*texture->height;

		const int x = glm::clamp(static_cast<int>(frag_tex_coord.x * texture->width), 0, texture->width);
		const int y = glm::clamp(static_cast<int>(frag_tex_coord.y * texture->height), 0, texture->height);
		
		glm::vec3 color(texture->data.get()[(texture->width * y + x)*texture->nComp], texture->data.get()[(texture->width * y + x)*texture->nComp + 1],
		                texture->data.get()[(texture->width * y + x)*texture->nComp + 2]);
		return color;
	}

	bool supportsBackFaceCulling() override
	{
		return true;
	}

	std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                              std::pair<float, TriangleSide> c) override { return nullptr; }
};
