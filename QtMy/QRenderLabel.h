#pragma once
#include <QLabel>
#include <QKeyEvent>
#include <glm/glm.hpp>

class QRenderLabel : public QLabel
{
public:
	explicit QRenderLabel(QWidget* parent = nullptr) : QLabel(parent)
	{
		m_timer_.setInterval(20);
		//mTimer.setSingleShot(true);
		connect(&m_timer_, &QTimer::timeout, [this]
		{
			QPoint this_glob_pos = mapToGlobal(pos());
			QPoint widget_center = QPoint(this_glob_pos.x() + width() / 2 - 150, this_glob_pos.y() + height() / 2);

			cam_rot_dir_ = glm::vec2(0);

			if (QCursor::pos() != widget_center)
			{
				QPoint mouse_dir = QCursor::pos() - widget_center;
				mouse_dir.setY(-mouse_dir.y());
				cam_rot_dir_ = glm::vec2(mouse_dir.x(), mouse_dir.y());
			}

			SetCursorPos(widget_center.x(), widget_center.y());
		});
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
		if (event->key() == 16777216)
		{
			m_timer_.stop();
			clearFocus();
			cam_rot_dir_ = glm::vec2(0);
			cam_move_dir_ = glm::vec3(0);
			setCursor(QCursor(Qt::ArrowCursor));
		}

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

	void focusInEvent(QFocusEvent* ev) override
	{
		QLabel::focusInEvent(ev);
		m_timer_.start();

		QPoint this_glob_pos = mapToGlobal(pos());
		QPoint widget_center = QPoint(this_glob_pos.x() + width() / 2 - 150, this_glob_pos.y() + height() / 2);
		SetCursorPos(widget_center.x(), widget_center.y());

		setCursor(QCursor(Qt::BlankCursor));
	}

private:
	glm::vec3 cam_move_dir_;
	glm::vec2 cam_rot_dir_;
	QTimer m_timer_;
};
