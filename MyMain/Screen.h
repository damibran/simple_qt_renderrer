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
	//thread_pool pool_;

	Screen(const uint mx, const uint my, uint _thread_count) : XMAX(mx / 2), YMAX(my / 2),
	                                                           buffer_(XMAX * 2, YMAX * 2, QImage::Format_RGB32),
	raw_buffer(reinterpret_cast<uint*>(buffer_.scanLine(0)))
	//,pool_(_thread_count)
	{
		//pool_.sleep_duration=0;
		for (size_t i = 0; i < XMAX * YMAX; ++i)
			z_buffer_.push_back(FLT_MAX);
	}

	void put_point(const uint x, const uint y, const glm::vec3& color)
	{
		//colorBuffer[(YMAX - y) * XMAX + x] = color;
		setpixInRawBuffer(x * 2u, (YMAX - y - 1) * 2u, qRgb(color.r, color.g, color.b));
		setpixInRawBuffer(x * 2u + 1, (YMAX - y - 1) * 2u, qRgb(color.r, color.g, color.b));
		setpixInRawBuffer(x * 2u, (YMAX - y - 1) * 2u + 1, qRgb(color.r, color.g, color.b));
		setpixInRawBuffer(x * 2u + 1, (YMAX - y - 1) * 2u + 1, qRgb(color.r, color.g, color.b));
	}

	QImage& getImage()
	{
		return buffer_;
	}

	void clearScreen()
	{
		//buffer_.fill(QColor(200, 200, 200));
		for (size_t i = 0; i < XMAX * YMAX * 4; ++i)
			raw_buffer[i] = qRgb(200, 200, 200);
		for (size_t i = 0; i < XMAX * YMAX; ++i)
			z_buffer_[i] = FLT_MAX;
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

	void setpixInRawBuffer(const uint x, const uint y, QRgb c)
	{
		raw_buffer[y * buffer_.width() + x] = c;
	}
};

#endif // SCREEN_H
