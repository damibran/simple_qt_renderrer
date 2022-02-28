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
	Screen(int mx, int my) :XMAX(mx), YMAX(my), buffer(std::make_unique<QImage>(XMAX, YMAX, QImage::Format_RGB32))
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

	float getZBufferAt(int i)
	{
		return zBuffer[i];
	}

	void setZBufferAt(int i, float val)
	{
		zBuffer[i] = val;
	}

private:

	std::unique_ptr<QImage> buffer;
	std::vector<float> zBuffer;
};

#endif // SCREEN_H
