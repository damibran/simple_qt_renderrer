#include "RendererMainWindow.h"
#include <glm/glm.hpp>

RendererMainWindow::RendererMainWindow(const int wr, const int hr, QWidget* parent)
	: QMainWindow(parent)
	,render_thread_(wr, hr,4,scene_,working_)
{
	ui_.setupUi(this);

	resize(wr + 250, hr);

	ui_.renderLabel->resize(wr, hr);

	scene_.setupScene(ui_,render_thread_.pool_);

	connect(&render_thread_.screen_,&Screen::ImageUpdated,this,&RendererMainWindow::printImage);

	render_thread_.start();
}

void RendererMainWindow::screen_refresh()
{
	///calculating delta time
	//qDebug() << 1.0f / delta_time;
}

void RendererMainWindow::printImage(const QImage& img,semaphore* s) const
{
	ui_.renderLabel->setPixmap(QPixmap::fromImage(img.scaled(ui_.renderLabel->size())));
	s->release();
}

void RendererMainWindow::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == 16777216)
	{
		working_=false;
		//this->close();
	}
}
