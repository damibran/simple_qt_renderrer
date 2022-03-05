#pragma once
#include"../utils/MVP_mat.h"
#include "screen.h"
#include "../shapes/shape.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera :public Shape
{
public:
	Camera(Screen& s) :screen(s)
	{
		rotation = glm::vec3(0, -90, 0);
		proj = glm::perspective(glm::radians(45.0f), (float)screen.XMAX / (float)screen.YMAX, 0.1f, 500.0f);
		updateCameraVectors();
	}
	void moveCamera(glm::vec3 dir, float dt)
	{
		float tspeed = dt * speed;
		glm::mat3 m = glm::mat3(Right, Up, Front);
		position += tspeed * m * dir;
	}
	void rotateCamera(glm::vec2 mouseDir)
	{
		float xoffset = mouseDir.x;
		float yoffset = mouseDir.y;

		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		rotation.y += xoffset;
		rotation.x += yoffset;

		bool constrainPitch = true;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (rotation.x > 89.0f)
				rotation.x = 89.0f;
			if (rotation.x < -89.0f)
				rotation.x = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}
	MVP_mat getCameraProjViewMat()const
	{
		MVP_mat vp;
		vp.proj = proj;
		vp.view = glm::lookAt(position, position + Front, Up);

		return vp;
	}
private:
	Screen& screen;
	glm::mat4 proj;
	glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Right;
	glm::vec3 WorldUp = glm::vec3(0, 1, 0);
	float MouseSensitivity = 0.25f;
	float speed = 30;

	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
		front.y = sin(glm::radians(rotation.x));
		front.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};