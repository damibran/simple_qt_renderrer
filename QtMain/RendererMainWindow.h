#pragma once

#include <glm/glm.hpp>
#include "../utils/RenderThread.h"
#include "../MyMain/Screen.h"
#include "../utils/Semaphore.h" 
#include "../MyMain/Scene.h"
#include "ui_RenderrerMainWindow.h"

class RendererMainWindow final : public QMainWindow
{
	Q_OBJECT

public:
	RendererMainWindow(int wr, int hr, QWidget* parent = Q_NULLPTR);

public slots:
	void screen_refresh();
	void printImage(const QImage&,semaphore* ) const;

protected:
	void keyPressEvent(QKeyEvent* event) override;

private:
	Ui::RenderrerMainWindowClass ui_;
	Scene scene_;
	RenderThread render_thread_;
	bool working_ = true;
};
