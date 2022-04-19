#ifndef SCREEN_H
#define SCREEN_H
#include <QtGui>
#include "ui_RenderrerMainWindow.h"
#include<glm/glm.hpp>
#include "../utils/thread_pool.hpp"

class Screen:public QObject
{
	Q_OBJECT

	signals:
	void ImageUpdated(QImage&);

public:
	const uint XMAX;
	const uint YMAX;
	thread_pool pool_;
	uint cur_buffer_ = 0;
	uint prev_buffer=1;

	Screen(const uint mx, const uint my, uint _thread_count) : XMAX(mx / 2), YMAX(my / 2),
	                                                           buffer_({
		                                                           QImage{
			                                                           static_cast<int>(XMAX * 2),
			                                                           static_cast<int>(YMAX * 2), QImage::Format_RGB32
		                                                           },
		                                                           QImage{
			                                                           static_cast<int>(XMAX * 2),
			                                                           static_cast<int>(YMAX * 2), QImage::Format_RGB32
		                                                           }
	                                                           }),
	                                                           pool_(XMAX, YMAX, _thread_count)
	{
		pool_.sleep_duration = 0;
	}

	void put_point(const uint a, const uint b, const glm::vec3& color)
	{
		buffer_[cur_buffer_].setPixel(a * 2u, (YMAX - b - 1) * 2u, qRgb(color.r, color.g, color.b));
		buffer_[cur_buffer_].setPixel(a * 2u + 1, (YMAX - b - 1) * 2u, qRgb(color.r, color.g, color.b));
		buffer_[cur_buffer_].setPixel(a * 2u, (YMAX - b - 1) * 2u + 1, qRgb(color.r, color.g, color.b));
		buffer_[cur_buffer_].setPixel(a * 2u + 1, (YMAX - b - 1) * 2u + 1, qRgb(color.r, color.g, color.b));
		//colorBuffer[(YMAX - b) * XMAX + a] = color;
	}

	void clearScreen()
	{
		pool_.clearBuffers(prev_buffer, {200, 200, 200});
	}

	void sumUpBuffers()
	{
		for (uint x = 0; x < XMAX; ++x)
		{
			for (uint y = 0; y < YMAX; ++y)
			{
				put_point(x, y, pool_.getThreadsColor(prev_buffer, x, y));
			}
		}
		emit ImageUpdated(buffer_[prev_buffer]);
	}

private:
	std::array<QImage, 2> buffer_;
};

#endif // SCREEN_H
