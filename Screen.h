#ifndef SCREEN_H
#define SCREEN_H
#include <QtGui>
#include<QColor>
#include<vector>
#include"Shaders/Shader.h"
#include<glm/glm.hpp>
#include "thread_pool.hpp"

class Screen
{
public:
	const int XMAX;
	const int YMAX;
	Screen(int mx, int my) :XMAX(mx), YMAX(my), buffer(std::make_unique<QImage>(XMAX, YMAX, QImage::Format_RGB32)), pool(6)
	{
		buffer->fill(QColor(150, 150, 150));
		for (size_t i = 0; i < XMAX * YMAX; ++i)
			zBuffer.push_back(FLT_MAX);
	}

	void put_point(int a, int b, glm::vec3 color)
	{

		buffer->setPixel(a, YMAX - b, qRgb(color.r, color.g, color.b));
		//colorBuffer[(YMAX - b) * XMAX + a] = color;
	}

	std::unique_ptr<QImage>& getImage()
	{
		return buffer;
	}

	void clearScreen()
	{
		buffer->fill(QColor(150, 150, 150));
		for (size_t i = 0; i < XMAX * YMAX; ++i)
			zBuffer[i] = FLT_MAX;
	}

	void process_trngl(std::unique_ptr<Shader>& shader, const MVP_mat& trans, const vertex& v0, const vertex& v1, const vertex& v2)
	{
		triangleClipPos abc = shader->computeVertexShader(trans, v0, v1, v2);

		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 c;

		//calculating raster positions
		a.x = (abc.v1)[0] / (abc.v1)[3] * XMAX + XMAX / 2;
		a.y = (abc.v1)[1] / (abc.v1)[3] * YMAX + YMAX / 2;
		a.z = abc.v1.w;

		b.x = (abc.v2)[0] / (abc.v2)[3] * XMAX + XMAX / 2;
		b.y = (abc.v2)[1] / (abc.v2)[3] * YMAX + YMAX / 2;
		b.z = abc.v2.w;

		c.x = (abc.v3)[0] / (abc.v3)[3] * XMAX + XMAX / 2;
		c.y = (abc.v3)[1] / (abc.v3)[3] * YMAX + YMAX / 2;
		c.z = abc.v3.w;

		if (abc.v1.z <= abc.v1.w && abc.v1.z >= -abc.v1.w &&
			abc.v2.z <= abc.v2.w && abc.v2.z >= -abc.v2.w &&
			abc.v3.z <= abc.v3.w && abc.v3.z >= -abc.v3.w)//kinda_Cliping
			put_triangle(shader, a, b, c);
	}
private:

	std::unique_ptr<QImage> buffer;
	std::vector<float> zBuffer;
	thread_pool pool;

	void put_triangle(std::unique_ptr<Shader>& shader, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
	{
		float xmin = min3(v0.x, v1.x, v2.x);
		float ymin = min3(v0.y, v1.y, v2.y);
		float xmax = max3(v0.x, v1.x, v2.x);
		float ymax = max3(v0.y, v1.y, v2.y);

		if (xmin < XMAX - 1 && xmax > 0 && ymin < YMAX - 1 && ymax > 0)
		{
			int x0 = std::max(0, (int)(std::floor(xmin)));
			int x1 = std::min(XMAX - 1, (int)(std::floor(xmax)));
			int y0 = std::max(1, (int)(std::floor(ymin)));
			int y1 = std::min(YMAX - 1, (int)(std::floor(ymax)));

			float area = edgeFunction(v0, v1, v2);
			for (int y = y0; y <= y1; ++y)
			{
				auto loop = [this, v0, v1, v2, area, y, &shader](const int a, const int b) {
					for (int x = a; x <= b; ++x)
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

							float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;
							if (z < zBuffer[y * XMAX + x])
							{
								zBuffer[y * XMAX + x] = z;

								glm::vec3 color = shader->computeFragmentShader(pixel, w0, w1, w2);

								color.r = glm::clamp<float>(color.r, 0, 255);
								color.g = glm::clamp<float>(color.g, 0, 255);
								color.b = glm::clamp<float>(color.b, 0, 255);

								put_point(x, y, color);
							}
						}
					}
				};
				pool.parallelize_loop(x0, x1, loop, (x1 - x0) / 6);
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
};

#endif // SCREEN_H
