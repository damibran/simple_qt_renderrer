#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <time.h>
#include <glm/glm.hpp>
#include "Screen.h"
#include "Scene.h"
#include "utils/input.h"

class MainWindow : public QWindow
{
	Q_OBJECT
public:
	explicit MainWindow(int wr, int hr, QWindow* parent = 0);

public slots:
	void screen_refresh();

protected:

	void resizeEvent(QResizeEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

private:
	std::chrono::system_clock::time_point tp1 = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point tp2 = std::chrono::system_clock::now();
	QBackingStore* m_backingStore;
	CameraAction camAct = CameraAction::NOTHING;
	int m_width;
	int m_height;
	Screen screen;
	Scene scene;
	QTimer* timer = new QTimer();

};
#endif // MAINWINDOW_H
