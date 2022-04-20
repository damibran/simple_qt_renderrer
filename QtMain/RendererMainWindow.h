#pragma once

#include <glm/glm.hpp>
#include "../MyMain/Screen.h"
#include "../MyMain/Scene.h"
#include "ui_RenderrerMainWindow.h"

class RenderThread
{
public:
	RenderThread(Screen& screen_,
	             Scene& scene_,
	             bool& working_): screen_(screen_), scene_(scene_), working_(working_)
	{
		t = std::thread(&RenderThread::work, this);
	}

	~RenderThread()
	{
		t.join();
	}

private:
	void work()
	{
		while (working_)
		{
			tp2_ = std::chrono::system_clock::now();
			const std::chrono::duration<float> elapsed_time = tp2_ - tp1_;
			tp1_ = tp2_;
			const float delta_time = elapsed_time.count();

			screen_.clearScreen();
			//updating all scene
			scene_.updateScene(delta_time);
			scene_.renderScene();
		}
	}

	std::chrono::system_clock::time_point tp1_ = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point tp2_ = std::chrono::system_clock::now();
	Screen& screen_;
	Scene& scene_;
	bool& working_;
	std::thread t;
};

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
	Screen screen_;
	Scene scene_;
	RenderThread render_thread_;
	bool working_ = true;
};
