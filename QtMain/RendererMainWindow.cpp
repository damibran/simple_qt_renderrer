#include "RendererMainWindow.h"
#include <glm/glm.hpp>

RendererMainWindow::RendererMainWindow(const int wr, const int hr, QWidget* parent)
	: QMainWindow(parent)
	  , screen_(wr, hr,4)
	  , scene_(screen_)
{
	ui_.setupUi(this);

	scene_.setupScene(ui_);

	resize(wr + 250, hr);

	ui_.renderLabel->resize(wr, hr);

	connect(&screen_,&Screen::ImageUpdated,this,&RendererMainWindow::printImage);

	render_thread_=std::thread([this]()
	{
		while(working_)
		{
			tp2_ = std::chrono::system_clock::now();
			const std::chrono::duration<float> elapsed_time = tp2_ - tp1_;
			tp1_ = tp2_;
			const float delta_time = elapsed_time.count();

			screen_.clearScreen();
			//updating all scene
			scene_.updateScene(delta_time);
			scene_.renderScene();
		}
	});
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
		render_thread_.join();
		//this->close();
	}
}
