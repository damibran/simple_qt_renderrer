#pragma once

#include <glm/glm.hpp>
#include "../MyMain/Screen.h"
#include "../MyMain/Scene.h"
#include "ui_RenderrerMainWindow.h"

class RendererMainWindow final : public QMainWindow
{
	Q_OBJECT

public:
	RendererMainWindow(int wr, int hr, QWidget* parent = Q_NULLPTR);

public slots:
	void screen_refresh();
	void printImage(const QImage&) const;

protected:
	void keyPressEvent(QKeyEvent* event) override;

private:
	Ui::RenderrerMainWindowClass ui_;
	std::chrono::system_clock::time_point tp1_ = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point tp2_ = std::chrono::system_clock::now();
	Screen screen_;
	Scene scene_;
	QTimer timer_;
};
