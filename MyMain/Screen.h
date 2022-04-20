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

	Screen(const uint mx, const uint my) : XMAX(mx / 2), YMAX(my / 2),
	                                                           buffer_({
		                                                           QImage{
			                                                           static_cast<int>(XMAX * 2),
			                                                           static_cast<int>(YMAX * 2), QImage::Format_RGB32
		                                                           },
		                                                           QImage{
			                                                           static_cast<int>(XMAX * 2),
			                                                           static_cast<int>(YMAX * 2), QImage::Format_RGB32
		                                                           }
	                                                           }){
	}

	void put_point(uint cur_buffer,const uint a, const uint b, const glm::vec3& color)
	{
		buffer_[cur_buffer].setPixel(a * 2u, (YMAX - b - 1) * 2u, qRgb(color.r, color.g, color.b));
		buffer_[cur_buffer].setPixel(a * 2u + 1, (YMAX - b - 1) * 2u, qRgb(color.r, color.g, color.b));
		buffer_[cur_buffer].setPixel(a * 2u, (YMAX - b - 1) * 2u + 1, qRgb(color.r, color.g, color.b));
		buffer_[cur_buffer].setPixel(a * 2u + 1, (YMAX - b - 1) * 2u + 1, qRgb(color.r, color.g, color.b));
		//colorBuffer[(YMAX - b) * XMAX + a] = color;
	}

	void sumUpBuffers(thread_pool& pool)
	{
		for (uint x = 0; x < XMAX; ++x)
		{
			for (uint y = 0; y < YMAX; ++y)
			{
				put_point(pool.prev_buffer,x, y, pool.getThreadsColor(pool.prev_buffer, x, y));
			}
		}
		emit ImageUpdated(buffer_[pool.prev_buffer]);
	}

private:
	std::array<QImage, 2> buffer_;
};

#endif // SCREEN_H
