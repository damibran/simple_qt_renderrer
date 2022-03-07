#pragma once
#include <QLabel>
#include <QKeyEvent>
#include <glm/glm.hpp>

class QRenderLabel: public QLabel
{
public:
	explicit QRenderLabel(QWidget* parent = nullptr) : QLabel(parent) {
	}

	void keyReleaseEvent(QKeyEvent* event) override
	{
		if (event->key() == 'A')
			cam_move_dir_.x -= -1;
		else if (event->key() == 'D')
			cam_move_dir_.x -= 1;

		if (event->key() == 'S')
			cam_move_dir_.y -= -1;
		else if (event->key() == 'W')
			cam_move_dir_.y -= 1;

		if (event->key() == 'E')
			cam_move_dir_.z -= 1;
		else if (event->key() == 'Q')
			cam_move_dir_.z -= -1;
	}

	void keyPressEvent(QKeyEvent* event) override
	{

		if (event->key() == 'A')
			cam_move_dir_.x += -1;
		else if (event->key() == 'D')
			cam_move_dir_.x += 1;

		if (event->key() == 'S')
			cam_move_dir_.y += -1;
		else if (event->key() == 'W')
			cam_move_dir_.y += 1;

		if (event->key() == 'E')
			cam_move_dir_.z += 1;
		else if (event->key() == 'Q')
			cam_move_dir_.z += -1;
	}

	void mouseMoveEvent(QMouseEvent* event) override
	{
		if (mouse_pos_ == QPoint(-1, -1))
			mouse_pos_ = event->pos();

		QPoint mouse_dir = event->pos() - mouse_pos_;
		mouse_dir.setY(-mouse_dir.y());
		mouse_pos_ = event->pos();

		glm::vec2 m;
		if (mouse_dir.x() != 0 || mouse_dir.y() != 0)
			m = normalize(glm::vec2(mouse_dir.x(), mouse_dir.y()));

		else
			m = glm::vec2(mouse_dir.x(), mouse_dir.y());


		cam_rot_dir_ = m;
	}

	glm::vec3 getMoveDir() const
	{
		return cam_move_dir_;
	}

	glm::vec2 getRotDir() const
	{
		return cam_rot_dir_;
	}

protected:
	void mouseReleaseEvent(QMouseEvent* ev) override
	{
		QLabel::mouseReleaseEvent(ev);

		cam_rot_dir_ = glm::vec2(0, 0);
	}
private:
	glm::vec3 cam_move_dir_;
	glm::vec2 cam_rot_dir_;
	QPoint mouse_pos_ = QPoint(-1, -1);

};
