#pragma once
#include "../MyMain/Screen.h"
#include "../utils/Mesh.h"
#include "../Renderers/ConcreteRenderers/ShaderMeshRenderer.h"
#include "../Shaders/Shader.h"

class ClipNearShaderRenderer : public ShaderMeshRenderer
{
public:
	ClipNearShaderRenderer(Screen& s, std::unique_ptr<Shader> shdr, const std::unique_ptr<Mesh>& m) : ShaderMeshRenderer(s,std::move(shdr),m)
	{}

	void process_trngl(std::unique_ptr<Shader>& shader, const Vertex& v0, const Vertex& v1,
	                   const Vertex& v2) override
	{
		TriangleClipPos abc = shader_->computeVertexShader(v0, v1, v2);

		if (abc.a.w < 0.1f)
		{
			if (abc.b.w < 0.1f)
			{
				if (abc.c.w < 0.1f)
				{
					// all outside
				}
				else
				{
					// ab outside c inside
					float t_ca = clipNearInHomoSpace(abc.c, abc.a);
					float t_bc = clipNearInHomoSpace(abc.b, abc.c);

					oneInsideTwoOutside(t_ca, TriangleSide::CA, t_bc, TriangleSide::BC, shader, abc);
				}
			}
			else
			{
				if (abc.c.w < 0.1f)
				{
					// ac outside b inside
					float t_ab = clipNearInHomoSpace(abc.a, abc.b);
					float t_bc = clipNearInHomoSpace(abc.b, abc.c);

					oneInsideTwoOutside(t_bc, TriangleSide::BC, t_ab, TriangleSide::AB, shader, abc);
				}
				else
				{
					// a outside cb inside
					float t_ab = clipNearInHomoSpace(abc.a, abc.b);
					float t_ca = clipNearInHomoSpace(abc.c, abc.a);

					twoInsideOneOutSide(t_ab, TriangleSide::AB, t_ca, TriangleSide::CA, shader, abc);
				}
			}
		}
		else
		{
			if (abc.b.w < 0.1f)
			{
				if (abc.c.w < 0.1f)
				{
					// bc outside a inside
					float t_ab = clipNearInHomoSpace(abc.a, abc.b);
					float t_ca = clipNearInHomoSpace(abc.c, abc.a);

					oneInsideTwoOutside(t_ab, TriangleSide::AB, t_ca, TriangleSide::CA, shader, abc);
				}
				else
				{
					// b outside ac inside
					float t_ab = clipNearInHomoSpace(abc.a, abc.b);
					float t_bc = clipNearInHomoSpace(abc.b, abc.c);

					twoInsideOneOutSide(t_bc, TriangleSide::BC, t_ab, TriangleSide::AB, shader, abc);
				}
			}
			else
			{
				if (abc.c.w < 0.1f)
				{
					// c outside ab inside
					float t_bc = clipNearInHomoSpace(abc.b, abc.c);
					float t_ca = clipNearInHomoSpace(abc.c, abc.a);

					twoInsideOneOutSide(t_ca, TriangleSide::CA, t_bc, TriangleSide::BC, shader, abc);
				}
				else
				{
					// all inside
					glm::vec3 a;
					glm::vec3 b;
					glm::vec3 c;

					//calculating raster positions
					a.x = static_cast<float>(screen_.XMAX) * ((abc.a.x / abc.a.w + 1) / 2);
					a.y = static_cast<float>(screen_.YMAX) * ((abc.a.y / abc.a.w + 1) / 2);
					a.z = abc.a.w;

					b.x = static_cast<float>(screen_.XMAX) * ((abc.b.x / abc.b.w + 1) / 2);
					b.y = static_cast<float>(screen_.YMAX) * ((abc.b.y / abc.b.w + 1) / 2);
					b.z = abc.b.w;

					c.x = static_cast<float>(screen_.XMAX) * ((abc.c.x / abc.c.w + 1) / 2);
					c.y = static_cast<float>(screen_.YMAX) * ((abc.c.y / abc.c.w + 1) / 2);
					c.z = abc.c.w;

					put_triangle(shader_, a, b, c);
				}
			}
		}
	}

private:
	//static float clipNearInHomoSpace(const glm::vec4& a, const glm::vec4& b)
	//// a is outside, b inside intrpolate from b to a
	//{
	//	return (b.w + b.z) / (b.w + b.z - (a.w + a.z));
	//}

	static float clipNearInHomoSpace(const glm::vec4& a, const glm::vec4& b)
	// a is outside, b inside intrpolate from a to b
	{
		return (a.w + a.z) / (a.w + a.z - (b.w + b.z));
	}

	static float clipNearInClipSpace(const glm::vec4& a, const glm::vec4& b) // a is outside, b inside
	{
		const float d_1 = -(b.z + 0.1f);
		const float d_2 = -(a.z + 0.1f);
		// eqval to  float d_2 = glm::dot(glm::vec3(a) - glm::vec3(0, 0, -0.1f), glm::vec3(0, 0, -1));
		return d_1 / (d_1 - d_2);
	}

	// {0 , side1 } - gives inside one and {1,side2} too
	void oneInsideTwoOutside(float t_side1, TriangleSide side1, float t_side2,
	                         TriangleSide side2, std::unique_ptr<Shader>& shader,
	                         const TriangleClipPos& abc)
	{
		const glm::vec4 v_side1 = abc.lerpSide(t_side1, side1);
		const glm::vec4 v_side2 = abc.lerpSide(t_side2, side2);

		std::unique_ptr<Shader> sh1 = shader->clone({t_side1, side1}, {t_side2, side2},
		                                            {0, side1});

		const glm::vec4 v_side3 = abc.lerpSide(0, side1);

		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 c;

		//calculating raster positions
		a.x = static_cast<float>(screen_.XMAX) * ((v_side1.x / v_side1.w + 1) / 2);
		a.y = static_cast<float>(screen_.YMAX) * ((v_side1.y / v_side1.w + 1) / 2);
		a.z = v_side1.w;

		b.x = static_cast<float>(screen_.XMAX) * ((v_side2.x / v_side2.w + 1) / 2);
		b.y = static_cast<float>(screen_.YMAX) * ((v_side2.y / v_side2.w + 1) / 2);
		b.z = v_side2.w;

		c.x = static_cast<float>(screen_.XMAX) * ((v_side3.x / v_side3.w + 1) / 2);
		c.y = static_cast<float>(screen_.YMAX) * ((v_side3.y / v_side3.w + 1) / 2);
		c.z = v_side3.w;

		put_triangle(sh1, a, b, c);
	}

	// {0,side1} gives outside one and {1, side2} too
	void twoInsideOneOutSide(float t_side1, TriangleSide side1, float t_side2,
	                         TriangleSide side2, std::unique_ptr<Shader>& shader,
	                         const TriangleClipPos& abc)
	{
		glm::vec4 v_side1 = abc.lerpSide(t_side1, side1);
		glm::vec4 v_side2 = abc.lerpSide(t_side2, side2);

		std::unique_ptr<Shader> sh1 = shader->clone({t_side1, side1}, {1, side1},
		                                            {0, side2});
		std::unique_ptr<Shader> sh2 = shader->clone({t_side1, side1}, {0, side2},
		                                            {t_side2, side2});

		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 c;

		v_side2 = abc.lerpSide(1, side1);
		glm::vec4 v_side3 = abc.lerpSide(0, side2);

		//calculating raster positions
		a.x = static_cast<float>(screen_.XMAX) * ((v_side1.x / v_side1.w + 1) / 2);
		a.y = static_cast<float>(screen_.YMAX) * ((v_side1.y / v_side1.w + 1) / 2);
		a.z = v_side1.w;

		b.x = static_cast<float>(screen_.XMAX) * ((v_side2.x / v_side2.w + 1) / 2);
		b.y = static_cast<float>(screen_.YMAX) * ((v_side2.y / v_side2.w + 1) / 2);
		b.z = v_side2.w;

		c.x = static_cast<float>(screen_.XMAX) * ((v_side3.x / v_side3.w + 1) / 2);
		c.y = static_cast<float>(screen_.YMAX) * ((v_side3.y / v_side3.w + 1) / 2);
		c.z = v_side3.w;

		put_triangle(sh1, a, b, c);

		v_side2 = abc.lerpSide(0, side2);
		v_side3 = abc.lerpSide(t_side2, side2);

		a.x = static_cast<float>(screen_.XMAX) * ((v_side1.x / v_side1.w + 1) / 2);
		a.y = static_cast<float>(screen_.YMAX) * ((v_side1.y / v_side1.w + 1) / 2);
		a.z = v_side1.w;

		b.x = static_cast<float>(screen_.XMAX) * ((v_side2.x / v_side2.w + 1) / 2);
		b.y = static_cast<float>(screen_.YMAX) * ((v_side2.y / v_side2.w + 1) / 2);
		b.z = v_side2.w;

		c.x = static_cast<float>(screen_.XMAX) * ((v_side3.x / v_side3.w + 1) / 2);
		c.y = static_cast<float>(screen_.YMAX) * ((v_side3.y / v_side3.w + 1) / 2);
		c.z = v_side3.w;

		put_triangle(sh2, a, b, c);
	}
};
