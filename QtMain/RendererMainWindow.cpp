#include "RendererMainWindow.h"
#include <glm/glm.hpp>

RendererMainWindow::RendererMainWindow(const int wr, const int hr, QWidget* parent)
	: QMainWindow(parent)
	  , screen_(wr, hr,4)
	  , scene_(screen_)
	,render_thread_(screen_,scene_,working_)
{
	ui_.setupUi(this);

	scene_.setupScene(ui_);

	resize(wr + 250, hr);

	ui_.renderLabel->resize(wr, hr);

	connect(&screen_,&Screen::ImageUpdated,this,&RendererMainWindow::printImage);
}

void RendererMainWindow::screen_refresh()
{
	///calculating delta time
	//qDebug() << 1.0f / delta_time;
}

void RendererMainWindow::printImage(const QImage& img) const
{
	ui_.renderLabel->setPixmap(QPixmap::fromImage(img.scaled(ui_.renderLabel->size())));
}

void RendererMainWindow::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == 16777216)
	{
		working_=false;
		//this->close();
	}
}
