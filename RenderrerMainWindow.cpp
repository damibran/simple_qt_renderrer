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
	scene.updateCameraPos(deltaTime, camMoveDir);
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