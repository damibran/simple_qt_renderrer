#ifndef SCREEN_H
#define SCREEN_H
#include <QtGui>
#include<vector>
#include<glm/glm.hpp>
#include "../utils/thread_pool.hpp"

class Screen
{
public:
	const uint XMAX;
	const uint YMAX;
	thread_pool pool_;

	Screen(const uint mx, const uint my, uint _thread_count) : XMAX(mx / 2), YMAX(my / 2),
	                                                           buffer_(XMAX * 2, YMAX * 2, QImage::Format_RGB32),
	                                                           pool_(XMAX, YMAX, _thread_count)
	{
		pool_.sleep_duration = 0;
	}

	void put_point(const uint a, const uint b, const glm::vec3& color)
	{
		buffer_.setPixel(a * 2u, (YMAX - b - 1) * 2u, qRgb(color.r, color.g, color.b));
		buffer_.setPixel(a * 2u + 1, (YMAX - b - 1) * 2u, qRgb(color.r, color.g, color.b));
		buffer_.setPixel(a * 2u, (YMAX - b - 1) * 2u + 1, qRgb(color.r, color.g, color.b));
		buffer_.setPixel(a * 2u + 1, (YMAX - b - 1) * 2u + 1, qRgb(color.r, color.g, color.b));
		//colorBuffer[(YMAX - b) * XMAX + a] = color;
	}

	void sumUpBuffers()
	{
		for (uint x = 0; x < XMAX; ++x)
		{
			for (uint y = 0; y < YMAX; ++y)
			{
				put_point(x,y,pool_.getThreadsColor(x,y));
			}
		}
	}

	QImage& getImage()
	{
		return buffer_;
	}

	void clearScreen()
	{
		pool_.clearBuffers({200,200,200});
	}

private:
	QImage buffer_;
};

#endif // SCREEN_H
