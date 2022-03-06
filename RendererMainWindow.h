#pragma once

#include <glm/glm.hpp>
#include "Screen.h"
#include "Scene.h"
#include "ui_RenderrerMainWindow.h"

class RendererMainWindow final : public QMainWindow
{
	Q_OBJECT

public:
	RendererMainWindow(int wr, int hr, QWidget* parent = Q_NULLPTR);

public slots:
	void screen_refresh();
	void sliderValueChanged();
	void curveRotationChanged();


protected:
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent*)override;
	void mouseMoveEvent(QMouseEvent* event)override;

private:
	Ui::RenderrerMainWindowClass ui_;
	std::chrono::system_clock::time_point tp1_ = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point tp2_ = std::chrono::system_clock::now();
	glm::vec3 cam_move_dir_;
	Screen screen_;
	Scene scene_;
	QPoint mouse_pos_ = QPoint(-1, -1);
	std::unique_ptr<QTimer> timer_;
	std::array<glm::vec3, 5>* cntrl_pts_ptr_;
	bool bezier_path_need_update_ = true;
	void setUpSliders() const;
};
