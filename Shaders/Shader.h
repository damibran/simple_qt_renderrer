#pragma once
#include <utility>
#include "../utils/TriangleSides.h"
#include "../utils/MVPMat.h"
#include "../utils/Vertex.h"

struct TriangleClipPos
{
	TriangleClipPos(glm::vec4 v1_t, glm::vec4 v2_t, glm::vec4 v3_t) : a(v1_t), b(v2_t), c(v3_t)
	{
	}

	glm::vec4 a;
	glm::vec4 b;
	glm::vec4 c;
};

class Shader
{
public:
	virtual ~Shader() = default;
	virtual TriangleClipPos computeVertexShader(const MVPMat& trans, const Vertex& v0, const Vertex& v1,
	                                            const Vertex& v2) = 0;
	virtual glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) = 0; //return color
	virtual std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                                      std::pair<float, TriangleSide> c) =0;
	virtual bool supportsBackFaceCulling() =0;
};
