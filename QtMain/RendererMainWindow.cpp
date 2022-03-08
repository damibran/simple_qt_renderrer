#include "RendererMainWindow.h"
#include <glm/glm.hpp>

RendererMainWindow::RendererMainWindow(int wr, int hr, QWidget* parent)
	: QMainWindow(parent)
	  , screen_(wr, hr)
	  , scene_(screen_)
	  , timer_(new QTimer())
{
	timer_->start(32);
	connect(timer_.get(), SIGNAL(timeout()), this, SLOT(screen_refresh()));

	ui_.setupUi(this);

	scene_.setupScene(ui_);

	resize(wr + 250, hr + 250);

	ui_.renderLabel->resize(wr, hr);
}

void RendererMainWindow::screen_refresh()
{
	///calculating delta time
	tp2_ = std::chrono::system_clock::now();
	const std::chrono::duration<float> elapsed_time = tp2_ - tp1_;
	tp1_ = tp2_;
	const float delta_time = elapsed_time.count();

	//qDebug() << 1.0f / deltaTime;

	screen_.clearScreen();
	//updating all scene
	scene_.updateScene(delta_time);
	scene_.renderScene();

	//updating screen using color buffer info
	ui_.renderLabel->setPixmap(QPixmap::fromImage((*screen_.getImage()).scaled(ui_.renderLabel->size())));
}

void RendererMainWindow::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == 16777216)
		this->close();
}
