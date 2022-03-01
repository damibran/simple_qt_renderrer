#pragma once

#include <QtWidgets/QMainWindow>
#include <time.h>
#include <glm/glm.hpp>
#include "Screen.h"
#include "Scene.h"
#include "utils/input.h"
#include "ui_RenderrerMainWindow.h"

class RenderrerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	RenderrerMainWindow(int wr, int hr, QWidget* parent = Q_NULLPTR);

public slots:
	void screen_refresh();

protected:
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent*)override;
	void mouseMoveEvent(QMouseEvent* event)override;
	void mouseReleaseEvent(QMouseEvent* e)override;

private:
	Ui::RenderrerMainWindowClass ui;
	std::chrono::system_clock::time_point tp1 = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point tp2 = std::chrono::system_clock::now();
	CameraMoveAction camMovAct = CameraMoveAction::NOTHING;
	int m_width;
	int m_height;
	Screen screen;
	Scene scene;
	QPoint camPos = QPoint(-1, -1);
	QPoint mouseDir;
	std::unique_ptr<QTimer> timer;
};
