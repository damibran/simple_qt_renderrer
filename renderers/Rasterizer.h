#pragma once
#include"../Screen.h"
#include"../utils/MVP_mat.h"
#include"../utils/Vertex.h"
#include "../Shaders/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Rasterizer
{

public:
	Rasterizer(Screen& s,Shader& shdr) : screen(s),shader(shdr) {}
	void process_trngl(const MVP_mat& trans, const vertex& v0, const vertex& v1, const vertex& v2) const
	{		
		triangleClipPos abc = shader.computeVertexShader(trans, v0, v1, v2);

		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 c;

		//calculating raster positions
		a.x = (abc.v1)[0] / (abc.v1)[3] * screen.XMAX + screen.XMAX / 2;
		a.y = (abc.v1)[1] / (abc.v1)[3] * screen.YMAX + screen.YMAX / 2;
		a.z = abc.v1.w;

		b.x = (abc.v2)[0] / (abc.v2)[3] * screen.XMAX + screen.XMAX / 2;
		b.y = (abc.v2)[1] / (abc.v2)[3] * screen.YMAX + screen.YMAX / 2;
		b.z = abc.v2.w;

		c.x = (abc.v3)[0] / (abc.v3)[3] * screen.XMAX + screen.XMAX / 2;
		c.y = (abc.v3)[1] / (abc.v3)[3] * screen.YMAX + screen.YMAX / 2;
		c.z = abc.v3.w;

		if (abc.v1.z <= abc.v1.w && abc.v1.z >= -abc.v1.w &&
			abc.v2.z <= abc.v2.w && abc.v2.z >= -abc.v2.w &&
			abc.v3.z <= abc.v3.w && abc.v3.z >= -abc.v3.w)//kinda_Cliping
			put_triangle(a, b, c, glm::vec3(trans.view * glm::vec4(world_ligth_pos, 1.0f)));
	}
	glm::vec3 world_ligth_pos = glm::vec3(-60, 0, 0);
private:
	void put_triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& view_light_pos)const
	{
		float xmin = min3(v0.x, v1.x, v2.x);
		float ymin = min3(v0.y, v1.y, v2.y);
		float xmax = max3(v0.x, v1.x, v2.x);
		float ymax = max3(v0.y, v1.y, v2.y);

		if (xmin < screen.XMAX - 1 && xmax > 0 && ymin < screen.YMAX - 1 && ymax > 0)
		{
			int x0 = std::max(0, (int)(std::floor(xmin)));
			int x1 = std::min(screen.XMAX - 1, (int)(std::floor(xmax)));
			int y0 = std::max(1, (int)(std::floor(ymin)));
			int y1 = std::min(screen.YMAX - 1, (int)(std::floor(ymax)));

			float area = edgeFunction(v0, v1, v2);
			for (int y = y0; y <= y1; ++y)
			{
				for (int x = x0; x <= x1; ++x)
				{
					glm::vec2 pixel = { x + 0.5,y + 0.5 };
					float w0 = edgeFunction(v1, v2, pixel);
					float w1 = edgeFunction(v2, v0, pixel);
					float w2 = edgeFunction(v0, v1, pixel);
					if (w0 >= 0 && w1 >= 0 && w2 >= 0)
					{
						w0 /= area;
						w1 /= area;
						w2 /= area;

						float corr = w0 / v0.z + w1 / v1.z + w2 / v2.z;

						w0 /= v0.z;
						w1 /= v1.z;
						w2 /= v2.z;

						w0 /= corr;
						w1 /= corr;
						w2 /= corr;

                        float color = shader.computeFragmentShader(pixel, w0, w1, w2);

						screen.put_point(x, y, glm::vec3(255.0f)*color);
					}
				}
			}
		}
	}
	float min3(const float& a, const float& b, const float& c)const
	{
		return std::min(a, std::min(b, c));
	}
	float max3(const float& a, const float& b, const float& c)const
	{
		return std::max(a, std::max(b, c));
	}
	float edgeFunction(const glm::vec3& a, const  glm::vec3& b, const  glm::vec2& c)const
	{
		return -((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x));
	}
    Screen& screen;
	Shader& shader;
};
