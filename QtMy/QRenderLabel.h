#pragma once
#include <QLabel>
#include <QKeyEvent>
#include <glm/glm.hpp>

class QRenderLabel : public QLabel
{
public:
	explicit QRenderLabel(QWidget* parent = nullptr) : QLabel(parent)
	{
		mTimer.setInterval(20);
		//mTimer.setSingleShot(true);
		connect(&mTimer, &QTimer::timeout, [this]
		{
			cam_rot_dir_ = glm::vec2(0);
			mouse_pos_ = QPoint(-1, -1);
			QPoint this_glob_pos = mapToGlobal(pos());
			SetCursorPos(this_glob_pos.x() + width() / 2 - 150, this_glob_pos.y() + height() / 2);
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
			mTimer.stop();
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

	void mouseMoveEvent(QMouseEvent* event) override
	{
		QLabel::mouseMoveEvent(event);
		if (mouse_pos_ == QPoint(-1, -1))
			mouse_pos_ = event->pos();

		QPoint mouse_dir = event->pos() - mouse_pos_;
		mouse_dir.setY(-mouse_dir.y());
		mouse_pos_ = event->pos();

		cam_rot_dir_ = glm::vec2(mouse_dir.x(), mouse_dir.y());
	}

	void mouseReleaseEvent(QMouseEvent* ev) override
	{
		QLabel::mouseReleaseEvent(ev);

		QPoint this_glob_pos = mapToGlobal(pos());
		SetCursorPos(this_glob_pos.x() + width() / 2 - 150, this_glob_pos.y() + height() / 2);
	}

	void focusInEvent(QFocusEvent* ev) override
	{
		mTimer.start();
		QLabel::focusInEvent(ev);
		setCursor(QCursor(Qt::BlankCursor));
	}

private:
	glm::vec3 cam_move_dir_;
	glm::vec2 cam_rot_dir_;
	QPoint mouse_pos_ = QPoint(-1, -1);
	QTimer mTimer;
};
