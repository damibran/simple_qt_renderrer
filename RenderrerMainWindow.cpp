#include "RenderrerMainWindow.h"
#include <glm/glm.hpp>

RenderrerMainWindow::RenderrerMainWindow(int wr, int hr, QWidget* parent)
	: QMainWindow(parent)
	, screen(wr / 2, hr / 2)
	, scene(screen)
	, timer(new QTimer())
{
	timer->start(16);
	QObject::connect(timer.get(), SIGNAL(timeout()), this, SLOT(screen_refresh()));

	ui.setupUi(this);

	resize(wr+250, hr);

	ui.renderLabel->resize(wr, hr);
}

void RenderrerMainWindow::screen_refresh()
{
	///calculating delta time
	tp2 = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime = tp2 - tp1;
	tp1 = tp2;
	float deltaTime = elapsedTime.count();

	qDebug() << 1.0f / deltaTime;

	//qDebug() << ui.renderLabel->size().width();

	//configure painter
	float system_scale = 2; //take into account system window elements scaling 1 - 100%,2 - 200%;

	screen.clearScreen();
	//updating all scene
	scene.updateCamera(camAct);
	camAct = CameraAction::NOTHING;
	scene.updateScene(deltaTime, { ui.XlineEdit->text().toFloat(), ui.YlineEdit->text().toFloat(),ui.ZlineEdit->text().toFloat() });

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