#pragma once
#include "../utils/MVP_mat.h"
#include "../utils/Vertex.h"

struct triangleClipPos
{
	triangleClipPos(glm::vec4 _v1, glm::vec4 _v2, glm::vec4 _v3) :v1(_v1), v2(_v2), v3(_v3) {}
	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 v3;
};

class Shader
{
public:
	virtual triangleClipPos computeVertexShader(const MVP_mat& trans, const vertex& v0, const vertex& v1, const vertex& v2)  = 0;
	virtual glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) = 0;//return color
	virtual std::unique_ptr<Shader> prototypeCopy() = 0;
};