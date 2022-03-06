#include "RendererMainWindow.h"
#include <glm/glm.hpp>

RendererMainWindow::RendererMainWindow(int wr, int hr, QWidget* parent)
	: QMainWindow(parent)
	, screen_(wr / 2, hr / 2)
	, scene_(screen_, cntrl_pts_ptr_, bezier_path_need_update_)
	, timer_(new QTimer())
{
	timer_->start(32);
	QObject::connect(timer_.get(), SIGNAL(timeout()), this, SLOT(screen_refresh()));

	ui_.setupUi(this);

	QObject::connect(ui_.XRotationValue, SIGNAL(editingFinished()), this, SLOT(curveRotationChanged()));
	QObject::connect(ui_.YRotationValue, SIGNAL(editingFinished()), this, SLOT(curveRotationChanged()));

	setUpSliders();

	resize(wr + 250, hr+250);

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

void RendererMainWindow::sliderValueChanged()
{
	bezier_path_need_update_ = true;

	(*cntrl_pts_ptr_)[0].x = ui_.P1XSlider->value();
	(*cntrl_pts_ptr_)[0].y = ui_.P1YSlider->value();
	(*cntrl_pts_ptr_)[0].z = ui_.P1ZSlider->value();

	(*cntrl_pts_ptr_)[1].x = ui_.P2XSlider->value();
	(*cntrl_pts_ptr_)[1].y = ui_.P2YSlider->value();
	(*cntrl_pts_ptr_)[1].z = ui_.P2ZSlider->value();

	(*cntrl_pts_ptr_)[2].x = ui_.P3XSlider->value();
	(*cntrl_pts_ptr_)[2].y = ui_.P3YSlider->value();
	(*cntrl_pts_ptr_)[2].z = ui_.P3ZSlider->value();

	(*cntrl_pts_ptr_)[3].x = ui_.P4XSlider->value();
	(*cntrl_pts_ptr_)[3].y = ui_.P4YSlider->value();
	(*cntrl_pts_ptr_)[3].z = ui_.P4ZSlider->value();

	(*cntrl_pts_ptr_)[4].x = ui_.P5XSlider->value();
	(*cntrl_pts_ptr_)[4].y = ui_.P5YSlider->value();
	(*cntrl_pts_ptr_)[4].z = ui_.P5ZSlider->value();
}

void RendererMainWindow::curveRotationChanged()
{
	scene_.setCurveRotation(ui_.XRotationValue->text().toFloat(), ui_.YRotationValue->text().toFloat());
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

void RendererMainWindow::setUpSliders() const
{
	QObject::connect(ui_.P1XSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui_.P1YSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui_.P1ZSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	QObject::connect(ui_.P2XSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui_.P2YSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui_.P2ZSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	QObject::connect(ui_.P3XSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui_.P3YSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui_.P3ZSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	QObject::connect(ui_.P4XSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui_.P4YSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui_.P4ZSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	QObject::connect(ui_.P5XSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui_.P5YSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui_.P5ZSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	(*cntrl_pts_ptr_)[0].x = ui_.P1XSlider->value();
	(*cntrl_pts_ptr_)[0].y = ui_.P1YSlider->value();
	(*cntrl_pts_ptr_)[0].z = ui_.P1ZSlider->value();

	(*cntrl_pts_ptr_)[1].x = ui_.P2XSlider->value();
	(*cntrl_pts_ptr_)[1].y = ui_.P2YSlider->value();
	(*cntrl_pts_ptr_)[1].z = ui_.P2ZSlider->value();

	(*cntrl_pts_ptr_)[2].x = ui_.P3XSlider->value();
	(*cntrl_pts_ptr_)[2].y = ui_.P3YSlider->value();
	(*cntrl_pts_ptr_)[2].z = ui_.P3ZSlider->value();

	(*cntrl_pts_ptr_)[3].x = ui_.P4XSlider->value();
	(*cntrl_pts_ptr_)[3].y = ui_.P4YSlider->value();
	(*cntrl_pts_ptr_)[3].z = ui_.P4ZSlider->value();

	(*cntrl_pts_ptr_)[4].x = ui_.P5XSlider->value();
	(*cntrl_pts_ptr_)[4].y = ui_.P5YSlider->value();
	(*cntrl_pts_ptr_)[4].z = ui_.P5ZSlider->value();
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
		m = glm::normalize(glm::vec2(mouse_dir.x(), mouse_dir.y()));

	else
		m = glm::vec2(mouse_dir.x(), mouse_dir.y());


	scene_.updateCameraRot(m);
}