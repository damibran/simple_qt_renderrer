#ifndef SCREEN_H
#define SCREEN_H
#include <QtGui>
#include<vector>
#include<algorithm>
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
	                                                           raw_buffer(reinterpret_cast<uint*>(buffer_.scanLine(0)))
	,pool_(_thread_count)
	{
		pool_.sleep_duration=0;
		z_buffer_.resize(XMAX * YMAX);
		std::fill(z_buffer_.begin(), z_buffer_.end(),FLT_MAX); 
	}

	void put_point(const uint x, const uint y, const glm::vec3& color)
	{
		//colorBuffer[(YMAX - y) * XMAX + x] = color;
		const QRgb qcolor = qRgb(255*color.r, 255*color.g, 255*color.b);
		setpixInRawBuffer(x * 2u, (YMAX - y - 1) * 2u, qcolor);
		setpixInRawBuffer(x * 2u + 1, (YMAX - y - 1) * 2u, qcolor);
		setpixInRawBuffer(x * 2u, (YMAX - y - 1) * 2u + 1, qcolor);
		setpixInRawBuffer(x * 2u + 1, (YMAX - y - 1) * 2u + 1, qcolor);
	}

	QImage& getImage()
	{
		return buffer_;
	}

	void clearScreen()
	{
		std::fill(raw_buffer,raw_buffer+XMAX * YMAX * 4,background_color);
		std::fill(z_buffer_.begin(), z_buffer_.end(),FLT_MAX);
	}

	float getZBufferAt(const uint i) const
	{
		return z_buffer_[i];
	}

	void setZBufferAt(const uint i, const float val)
	{
		z_buffer_[i] = val;
	}

private:
	QImage buffer_;
	uint* raw_buffer;
	std::vector<float> z_buffer_;
	QRgb background_color = qRgb(235, 235, 235);


	void setpixInRawBuffer(const uint x, const uint y, QRgb c)
	{
		raw_buffer[y * XMAX * 2 + x] = c;
	}
};

#endif // SCREEN_H
