#include "RendererMainWindow.h"
#include <glm/glm.hpp>

RendererMainWindow::RendererMainWindow(int wr, int hr, QWidget* parent)
	: QMainWindow(parent)
	  , screen_(wr / 2, hr / 2)
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
	scene_.updateCameraPos(delta_time, cam_move_dir_);
	scene_.updateScene(delta_time);
	scene_.renderScene();

	//updating screen using color buffer info
	ui_.renderLabel->setPixmap(QPixmap::fromImage((*screen_.getImage()).scaled(ui_.renderLabel->size())));
}

void RendererMainWindow::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == 16777216)
		this->close();

	if (event->key() == 'A')
		cam_move_dir_.x += -1;
	else if (event->key() == 'D')
		cam_move_dir_.x += 1;

	if (event->key() == 'S')
		cam_move_dir_.y += -1;
	else if (event->key() == 'W')
		cam_move_dir_.y += 1;

	if (event->key() == 'E')
		cam_move_dir_.z += 1;
	else if (event->key() == 'Q')
		cam_move_dir_.z += -1;
}

void RendererMainWindow::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == 'A')
		cam_move_dir_.x -= -1;
	else if (event->key() == 'D')
		cam_move_dir_.x -= 1;

	if (event->key() == 'S')
		cam_move_dir_.y -= -1;
	else if (event->key() == 'W')
		cam_move_dir_.y -= 1;

	if (event->key() == 'E')
		cam_move_dir_.z -= 1;
	else if (event->key() == 'Q')
		cam_move_dir_.z -= -1;
}

void RendererMainWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (mouse_pos_ == QPoint(-1, -1))
		mouse_pos_ = event->pos();

	QPoint mouse_dir = event->pos() - mouse_pos_;
	mouse_dir.setY(-mouse_dir.y());
	mouse_pos_ = event->pos();

	glm::vec2 m;
	if (mouse_dir.x() != 0 || mouse_dir.y() != 0)
		m = normalize(glm::vec2(mouse_dir.x(), mouse_dir.y()));

	else
		m = glm::vec2(mouse_dir.x(), mouse_dir.y());


	scene_.updateCameraRot(m);
}
