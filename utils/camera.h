#pragma once
#include"../utils/MVP_mat.h"
#include "input.h"
#include "screen.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 60.0f);

	Camera(Screen& s) :screen(s)
	{
		proj = glm::perspective(glm::radians(45.0f), (float)screen.XMAX / (float)screen.YMAX, 0.1f, 500.0f);
		updateCameraVectors();
	}
	void moveCamera(CameraMoveAction act, float dt)
	{
		float tspeed = dt * speed;
		if (act == CameraMoveAction::UP)
			cameraPos += tspeed * Up;
		else if (act == CameraMoveAction::DOWN)
			cameraPos -= tspeed * Up;
		else if (act == CameraMoveAction::LEFT)
			cameraPos -= Right * tspeed;
		else if (act == CameraMoveAction::RIGHT)
			cameraPos += Right * tspeed;
		else if (act == CameraMoveAction::ZOOMOUT)
			cameraPos -= tspeed * Front;
		else if (act == CameraMoveAction::ZOOMIN)
			cameraPos += tspeed * Front;
	}
	void moveCamera(CameraMoveAction act)
	{
		float tspeed = 0.16 * speed;
		if (act == CameraMoveAction::UP)
			cameraPos += tspeed * Up;
		else if (act == CameraMoveAction::DOWN)
			cameraPos -= tspeed * Up;
		else if (act == CameraMoveAction::LEFT)
			cameraPos -= Right * tspeed;
		else if (act == CameraMoveAction::RIGHT)
			cameraPos += Right * tspeed;
		else if (act == CameraMoveAction::ZOOMOUT)
			cameraPos -= tspeed * Front;
		else if (act == CameraMoveAction::ZOOMIN)
			cameraPos += tspeed * Front;
	}
	void rotateCamera(glm::vec2 mouseDir)
	{
		float xoffset = mouseDir.x;
		float yoffset = mouseDir.y;

		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		bool constrainPitch = true;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}
	MVP_mat getCameraProjViewMat()const
	{
		MVP_mat vp;
		vp.proj = proj;
		vp.view = glm::lookAt(cameraPos, cameraPos + Front, Up);

		return vp;
	}
private:
	Screen& screen;
	glm::mat4 proj;
	float Yaw = -90;
	float Pitch = 0;
	glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Right;
	glm::vec3 WorldUp = glm::vec3(0, 1, 0);
	float MouseSensitivity = 1.0f;
	float speed = 30;

	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};