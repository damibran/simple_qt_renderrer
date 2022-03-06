#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "screen.h"
#include "../shapes/shape.h"
#include"../utils/MVPMat.h"

class Camera :public Shape
{
public:
	explicit Camera(Screen& s) :screen_(s)
	{
		rotation_ = glm::vec3(0, -90, 0);
		proj_ = glm::perspective(glm::radians(45.0f), static_cast<float>(screen_.XMAX) / static_cast<float>(screen_.YMAX), 0.1f, 500.0f);
		updateCameraVectors();
	}
	void moveCamera(const glm::vec3 dir, const float dt)
	{
		const float tspeed = dt * speed_;
		const glm::mat3 m = glm::mat3(right_, up_, front_);
		position_ += tspeed * m * dir;
	}
	void rotateCamera(const glm::vec2 mouse_dir)
	{
		float xoffset = mouse_dir.x;
		float yoffset = mouse_dir.y;

		xoffset *= mouse_sensitivity_;
		yoffset *= mouse_sensitivity_;

		rotation_.y += xoffset;
		rotation_.x += yoffset;

		const bool constrainPitch = true;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (rotation_.x > 89.0f)
				rotation_.x = 89.0f;
			if (rotation_.x < -89.0f)
				rotation_.x = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	[[nodiscard]] MVPMat getCameraProjViewMat()const
	{
		MVPMat vp;
		vp.proj = proj_;
		vp.view = glm::lookAt(position_, position_ + front_, up_);

		return vp;
	}
private:
	Screen& screen_;
	glm::mat4 proj_;
	glm::vec3 front_ = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right_;
	glm::vec3 world_up_ = glm::vec3(0, 1, 0);
	float mouse_sensitivity_ = 0.25f;
	float speed_ = 30;

	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(rotation_.y)) * cos(glm::radians(rotation_.x));
		front.y = sin(glm::radians(rotation_.x));
		front.z = sin(glm::radians(rotation_.y)) * cos(glm::radians(rotation_.x));
		front_ = glm::normalize(front);
		// also re-calculate the Right and Up vector
		right_ = glm::normalize(glm::cross(front_, world_up_));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		up_ = glm::normalize(glm::cross(right_, front_));
	}
};
