#pragma once
#include "../MyMain/shape.h"
#include "../Shader.h"

class FunShader : public Shader
{
	struct vrtx
	{
		glm::vec2 TC;
	};

	vrtx a;
	vrtx b;
	vrtx c;

	glm::mat4 mvp;

	const Texture* texture;


	double& time;

public:

	FunShader(double& t):time(t){}

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

		const float speed = 5.0;
		const float f = pow(sin(20.0*frag_tex_coord.x + speed*time),2.0)+pow(cos(10.0*frag_tex_coord.y),2.0);

		const glm::vec3 shield_color = glm::vec3{abs(sin(f)),abs(cos(f)),0.5};//{abs(sin(2*time))*abs(sin(f)),abs(cos(2*time))*abs(cos(f)),abs(cos(time+sin(time)))}
		const glm::vec3 obj_color = texture->sampleTexture(frag_tex_coord);


		return obj_color + 0.3f*(shield_color - obj_color);
	}

	bool supportsBackFaceCulling() override
	{
		return true;
	}

	std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                              std::pair<float, TriangleSide> c) override { return nullptr; }
};
