#pragma once
#include "../utils/MVPMat.h"
#include "../utils/Vertex.h"

struct TriangleClipPos
{
	TriangleClipPos(glm::vec4 v1_t, glm::vec4 v2_t, glm::vec4 v3_t) :v1(v1_t), v2(v2_t), v3(v3_t) {}
	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 v3;
};

class Shader
{
public:
	virtual ~Shader() = default;
	virtual TriangleClipPos computeVertexShader(const MVPMat& trans, const Vertex& v0, const Vertex& v1, const Vertex& v2)  = 0;
	virtual glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) = 0;//return color
};