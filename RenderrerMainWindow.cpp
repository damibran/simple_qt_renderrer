#include "RenderrerMainWindow.h"
#include <glm/glm.hpp>

RenderrerMainWindow::RenderrerMainWindow(int wr, int hr, QWidget* parent)
	: QMainWindow(parent)
	, screen(wr / 2, hr / 2)
	, scene(screen)
	, timer(new QTimer())
{
	timer->start(32);
	QObject::connect(timer.get(), SIGNAL(timeout()), this, SLOT(screen_refresh()));

	ui.setupUi(this);

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
	scene.updateCamera(camAct);
	//camAct = CameraAction::NOTHING;
	scene.updateScene(deltaTime);
	scene.renderScene();

	glm::vec3 camPos = scene.getCamPosition();

	//updating screen using colorbuffer info
	ui.renderLabel->setPixmap(QPixmap::fromImage((*screen.getImage()).scaled(ui.renderLabel->size())));
}

void RenderrerMainWindow::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == 16777216)
		this->close();
	else if (event->key() == 'A')
		camAct = CameraAction::LEFT;
	else if (event->key() == 'W')
		camAct = CameraAction::UP;
	else if (event->key() == 'D')
		camAct = CameraAction::RIGHT;
	else if (event->key() == 'S')
		camAct = CameraAction::DOWN;
	else if (event->key() == 'E')
		camAct = CameraAction::ZOOMIN;
	else if (event->key() == 'Q')
		camAct = CameraAction::ZOOMOUT;
}

void RenderrerMainWindow::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == 'A' ||
		event->key() == 'W' ||
		event->key() == 'D' ||
		event->key() == 'S' ||
		event->key() == 'E' ||
		event->key() == 'Q')
		camAct = CameraAction::NOTHING;
}
