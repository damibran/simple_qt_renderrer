#include"mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(int wr, int hr, QWindow* parent)
	: QWindow(parent)
	, m_backingStore(new QBackingStore(this))
	, screen(wr, hr)
	, scene(screen)
{
	setGeometry(100, 100, wr * 3, hr * 3);
	m_width = wr;
	m_height = hr;

}

void MainWindow::screen_refresh()
{
	if (!isExposed())
		return;

	///calculating delta time
	tp2 = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime = tp2 - tp1;
	tp1 = tp2;
	float deltaTime = elapsedTime.count();

	qDebug() << 1.0f / deltaTime;

	///creating basic objs
	QRect rect(0, 0, width(), height());
	m_backingStore->beginPaint(rect);

	QPaintDevice* device = m_backingStore->paintDevice();
	QPainter painter(device);
	QPen screenPen;

	//configure painter
	float system_scale = 1; //take into account system window elements scaling 1 - 100%,2 - 200%;
	painter.setWindow(0, 0, m_width * system_scale, m_height * system_scale);

	screen.clearScreen();
	//updating all scene
	scene.updateCamera(camAct);
	camAct = CameraAction::NOTHING;
	scene.updateScene();

	//updating screen using colorbuffer info
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			screenPen.setColor(screen.getColorAtIndex(y * m_width + x));
			painter.setPen(screenPen);

			painter.drawPoint(x, y);
		}
	}
	//end of render loop
	painter.end();

	m_backingStore->endPaint();
	m_backingStore->flush(rect);
}

bool MainWindow::event(QEvent* event)
{
	if (event->type() == QEvent::UpdateRequest) {
		screen_refresh();
		return true;
	}
	return QWindow::event(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
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

void MainWindow::exposeEvent(QExposeEvent*)
{
	//if (isExposed())
	//    screen_refresh();
}

void MainWindow::resizeEvent(QResizeEvent* resizeEvent)
{
	QSize size = resizeEvent->size();
	m_backingStore->resize(size);
}
