#pragma once
#include <utility>
#include "../utils/TriangleSides.h"
#include "../utils/MVPMat.h"
#include "../utils/Vertex.h"
#include "../utils/Mesh.h"

struct TriangleClipPos
{
	TriangleClipPos(glm::vec4 v1_t, glm::vec4 v2_t, glm::vec4 v3_t) : a(v1_t), b(v2_t), c(v3_t)
	{
	}

	glm::vec4 lerpSide(float t, TriangleSide side) const
	{
		if (side == TriangleSide::AB)
		{
			return a + t * (b - a);
		}
		if (side == TriangleSide::BC)
		{
			return b + t * (c - b);
		}
		return c + t * (a - c);
	}

	glm::vec4 a;
	glm::vec4 b;
	glm::vec4 c;
};

class Shader
{
public:
	virtual ~Shader() = default;

	virtual void preparePerObjectVertexShaderData(const MVPMat& trans) =0;

	virtual TriangleClipPos computeVertexShader(const Vertex& v0,
	                                            const Vertex& v1,
	                                            const Vertex& v2) =0;

	virtual void preparePerMeshData(std::unique_ptr<Mesh> const& mesh)
	{
	}

	virtual glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) = 0; //return color
	virtual std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                                      std::pair<float, TriangleSide> c) =0;
	virtual bool supportsBackFaceCulling() =0;

	virtual void changeColor(glm::vec3 color)
	{
	}
};
