#ifndef SCREEN_H
#define SCREEN_H
#include <QtGui>
#include<vector>
#include<glm/glm.hpp>
#include "thread_pool.hpp"

class Screen
{
public:
	const uint XMAX;
	const uint YMAX;

	Screen(const uint mx, const uint my) : XMAX(mx), YMAX(my),
	                                       buffer_(std::make_unique<QImage>(XMAX, YMAX, QImage::Format_RGB32))
	{
		buffer_->fill(QColor(150, 150, 150));
		for (size_t i = 0; i < XMAX * YMAX; ++i)
			z_buffer_.push_back(FLT_MAX);
	}

	void put_point(const uint a, const uint b, const glm::vec3 color) const
	{
		buffer_->setPixel(a, YMAX - b - 1, qRgb(color.r, color.g, color.b));
		//colorBuffer[(YMAX - b) * XMAX + a] = color;
	}

	std::unique_ptr<QImage>& getImage()
	{
		return buffer_;
	}

	void clearScreen()
	{
		buffer_->fill(QColor(150, 150, 150));
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
	std::unique_ptr<QImage> buffer_;
	std::vector<float> z_buffer_;
};

#endif // SCREEN_H
