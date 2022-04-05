#pragma once
#include "../Script.h"
#include "ui_RenderrerMainWindow.h"
#include "../utils/Transform.h"
#include "../MyMain/Screen.h"
#include "../utils/MVPMat.h"

class CameraScript final : public Script
{
public:
	static std::unique_ptr<Shape> createObject(Ui::RenderrerMainWindowClass& ui, Screen& screen)
	{
		std::unique_ptr<Transform> t(new Transform);

		t->translate({ 0, 0, 60 });
		t->setRotationDegrees({ 0, -90, 0 });

		auto shp = std::make_unique<Shape>(std::move(t));

		shp->setScript(std::make_unique<CameraScript>(ui, screen, shp->getTransformPtr()));

		return shp;
	}

	CameraScript(Ui::RenderrerMainWindowClass& ui, Screen& s, std::unique_ptr<Transform>& transform) : ui_(ui), screen_(s),
		transform_(transform)
	{
		proj_ = glm::perspective(glm::radians(45.0f),
		                         static_cast<float>(screen_.XMAX) / static_cast<float>(screen_.YMAX), 0.1f, 500.0f);
		updateCameraVectors();
	}

	void updateScript(float dt) override
	{
		moveCamera(ui_.renderLabel->getMoveDir(), dt);

		rotateCamera(ui_.renderLabel->getRotDir());
	}

	[[nodiscard]] MVPMat getCameraProjViewMat() const
	{
		MVPMat vp;
		vp.proj = proj_;
		vp.view = glm::lookAt(transform_->getPos(), transform_->getPos() + front_, up_);

		return vp;
	}

private:
	void moveCamera(const glm::vec3 dir, const float dt)
	{
		const float tspeed = dt * speed_;
		const glm::mat3 m = glm::mat3(right_, up_, front_);
		transform_->translate(tspeed * m * dir);
	}

	void rotateCamera(const glm::vec2 mouse_dir)
	{
		glm::vec2 offset(mouse_dir.y, mouse_dir.x);

		offset *= mouse_sensitivity_;

		glm::vec3 t(transform_->getRotationDegrees());

		t += glm::vec3(offset, 0.);

		transform_->setRotationDegrees(t);

		const bool constrainPitch = true;

		float pitch = transform_->getRotationDegrees().x;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (pitch > 89.0f)
				transform_->setPitch(89.0f);
			if (pitch < -89.0f)
				transform_->setPitch(-89.0f);
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	Ui::RenderrerMainWindowClass& ui_;
	Screen& screen_;
	std::unique_ptr<Transform>& transform_;
	glm::mat4 proj_;
	glm::vec3 front_ = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right_;
	glm::vec3 world_up_ = glm::vec3(0, 1, 0);
	float mouse_sensitivity_ = 0.15f;
	float speed_ = 10;

	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		glm::vec3 rot(transform_->getRotationDegrees());
		front.x = cos(glm::radians(rot.y)) * cos(glm::radians(rot.x));
		front.y = sin(glm::radians(rot.x));
		front.z = sin(glm::radians(rot.y)) * cos(glm::radians(rot.x));
		front_ = glm::normalize(front);
		// also re-calculate the Right and Up vector
		right_ = glm::normalize(glm::cross(front_, world_up_));
		// normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		up_ = glm::normalize(glm::cross(right_, front_));
	}
};
