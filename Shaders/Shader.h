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

	virtual TriangleClipPos computeVertexShader(const MVPMat& trans, const glm::mat3 t_inv_VM, const Vertex& v0,
	                                            const Vertex& v1,
	                                            const Vertex& v2)
	{
		return computeVertexShader(trans, v0, v1, v2);
	}

	virtual TriangleClipPos computeVertexShader(const MVPMat& trans, const Vertex& v0,
	                                            const Vertex& v1,
	                                            const Vertex& v2)
	{
		glm::vec4 clip_a = trans.proj * trans.view * trans.model * glm::vec4(v0.pos, 1.0f);
		glm::vec4 clip_b = trans.proj * trans.view * trans.model * glm::vec4(v1.pos, 1.0f);
		glm::vec4 clip_c = trans.proj * trans.view * trans.model * glm::vec4(v2.pos, 1.0f);

		return TriangleClipPos(clip_a, clip_b, clip_c);
	}

	virtual glm::vec3 computeFragmentShader(const glm::vec2& pixel, float w0, float w1, float w2) = 0; //return color
	virtual std::unique_ptr<Shader> clone(std::pair<float, TriangleSide> a, std::pair<float, TriangleSide> b,
	                                      std::pair<float, TriangleSide> c) const =0;

	virtual std::unique_ptr<Shader> clone() const
	{
		return clone({0, TriangleSide::AB}, {0, TriangleSide::BC}, {0, TriangleSide::CA});
	};

	virtual bool supportsBackFaceCulling() =0;

	virtual void changeColor(glm::vec3 color)
	{
	};

	friend bool operator==(const Shader& lhs, const Shader& rhs)
	{
		return typeid(lhs) == typeid(rhs)
			&& lhs.isEqual(rhs);
	}

protected:
	[[nodiscard]] virtual bool isEqual(const Shader& obj) const=0;
};
