#include "RenderrerMainWindow.h"
#include <glm/glm.hpp>

RenderrerMainWindow::RenderrerMainWindow(int wr, int hr, QWidget* parent)
	: QMainWindow(parent)
	, screen(wr / 2, hr / 2)
	, scene(screen, cntrlPts_ptr, bezierPathNeedUpdate)
	, timer(new QTimer())
{
	timer->start(32);
	QObject::connect(timer.get(), SIGNAL(timeout()), this, SLOT(screen_refresh()));

	ui.setupUi(this);

	QObject::connect(ui.P1XSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui.P1YSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui.P1ZSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	QObject::connect(ui.P2XSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui.P2YSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui.P2ZSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	QObject::connect(ui.P3XSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui.P3YSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui.P3ZSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	QObject::connect(ui.P4XSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui.P4YSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui.P4ZSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	QObject::connect(ui.P5XSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui.P5YSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	QObject::connect(ui.P5ZSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));

	(*cntrlPts_ptr)[0].x = ui.P1XSlider->value();
	(*cntrlPts_ptr)[0].y = ui.P1YSlider->value();
	(*cntrlPts_ptr)[0].z = ui.P1ZSlider->value();

	(*cntrlPts_ptr)[1].x = ui.P2XSlider->value();
	(*cntrlPts_ptr)[1].y = ui.P2YSlider->value();
	(*cntrlPts_ptr)[1].z = ui.P2ZSlider->value();

	(*cntrlPts_ptr)[2].x = ui.P3XSlider->value();
	(*cntrlPts_ptr)[2].y = ui.P3YSlider->value();
	(*cntrlPts_ptr)[2].z = ui.P3ZSlider->value();

	(*cntrlPts_ptr)[3].x = ui.P4XSlider->value();
	(*cntrlPts_ptr)[3].y = ui.P4YSlider->value();
	(*cntrlPts_ptr)[3].z = ui.P4ZSlider->value();

	(*cntrlPts_ptr)[4].x = ui.P5XSlider->value();
	(*cntrlPts_ptr)[4].y = ui.P5YSlider->value();
	(*cntrlPts_ptr)[4].z = ui.P5ZSlider->value();

	resize(wr + 250, hr);

	ui.renderLabel->resize(wr, hr);
}

void RenderrerMainWindow::screen_refresh()
{
	///calculating delta time
	tp2 = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime = tp2 - tp1;
	tp1 = tp2;
	float deltaTime = elapsedTime.count();

	//qDebug() << 1.0f / deltaTime;

	screen.clearScreen();
	//updating all scene
	scene.updateCameraPos(deltaTime, camMoveDir);
	scene.updateScene(deltaTime);
	scene.renderScene();

	glm::vec3 camPos = scene.getCamPosition();

	//updating screen using colorbuffer info
	ui.renderLabel->setPixmap(QPixmap::fromImage((*screen.getImage()).scaled(ui.renderLabel->size())));
}

void RenderrerMainWindow::sliderValueChanged()
{
	bezierPathNeedUpdate = true;

	(*cntrlPts_ptr)[0].x = ui.P1XSlider->value();
	(*cntrlPts_ptr)[0].y = ui.P1YSlider->value();
	(*cntrlPts_ptr)[0].z = ui.P1ZSlider->value();

	(*cntrlPts_ptr)[1].x = ui.P2XSlider->value();
	(*cntrlPts_ptr)[1].y = ui.P2YSlider->value();
	(*cntrlPts_ptr)[1].z = ui.P2ZSlider->value();

	(*cntrlPts_ptr)[2].x = ui.P3XSlider->value();
	(*cntrlPts_ptr)[2].y = ui.P3YSlider->value();
	(*cntrlPts_ptr)[2].z = ui.P3ZSlider->value();

	(*cntrlPts_ptr)[3].x = ui.P4XSlider->value();
	(*cntrlPts_ptr)[3].y = ui.P4YSlider->value();
	(*cntrlPts_ptr)[3].z = ui.P4ZSlider->value();

	(*cntrlPts_ptr)[4].x = ui.P5XSlider->value();
	(*cntrlPts_ptr)[4].y = ui.P5YSlider->value();
	(*cntrlPts_ptr)[4].z = ui.P5ZSlider->value();
}

void RenderrerMainWindow::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == 16777216)
		this->close();

	if (event->key() == 'A')
		camMoveDir.x = -1;
	else if (event->key() == 'D')
		camMoveDir.x = 1;

	if (event->key() == 'S')
		camMoveDir.y = -1;
	else if (event->key() == 'W')
		camMoveDir.y = 1;

	if (event->key() == 'E')
		camMoveDir.z = 1;
	else if (event->key() == 'Q')
		camMoveDir.z = -1;
}

void RenderrerMainWindow::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == 'A' || event->key() == 'D')
		camMoveDir.x = 0;

	if (event->key() == 'S' || event->key() == 'W')
		camMoveDir.y = 0;

	if (event->key() == 'E' || event->key() == 'Q')
		camMoveDir.z = 0;
}

void RenderrerMainWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (mousePos == QPoint(-1, -1))
		mousePos = event->pos();

	QPoint mouseDir = event->pos() - mousePos;
	mouseDir.setY(-mouseDir.y());
	mousePos = event->pos();

	glm::vec2 m;
	if (mouseDir.x() != 0 || mouseDir.y() != 0)
		m = glm::normalize(glm::vec2(mouseDir.x(), mouseDir.y()));

	else
		m = glm::vec2(mouseDir.x(), mouseDir.y());


	scene.updateCameraRot(m);
}