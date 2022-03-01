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
	}
	void moveCamera(CameraAction act, float dt)
	{
		float tspeed = dt * speed;
		if (act == CameraAction::UP)
			cameraPos += tspeed * cameraUp;
		else if (act == CameraAction::DOWN)
			cameraPos -= tspeed * cameraUp;
		else if (act == CameraAction::LEFT)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * tspeed;
		else if (act == CameraAction::RIGHT)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * tspeed;
		else if (act == CameraAction::ZOOMOUT)
			cameraPos -= tspeed * cameraFront;
		else if (act == CameraAction::ZOOMIN)
			cameraPos += tspeed * cameraFront;
	}
	void moveCamera(CameraAction act)
	{
		float tspeed = 0.16 * speed;
		if (act == CameraAction::UP)
			cameraPos += tspeed * cameraUp;
		else if (act == CameraAction::DOWN)
			cameraPos -= tspeed * cameraUp;
		else if (act == CameraAction::LEFT)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * tspeed;
		else if (act == CameraAction::RIGHT)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * tspeed;
		else if (act == CameraAction::ZOOMOUT)
			cameraPos -= tspeed * cameraFront;
		else if (act == CameraAction::ZOOMIN)
			cameraPos += tspeed * cameraFront;
	}
	MVP_mat getCameraProjViewMat()const
	{
		MVP_mat vp;
		vp.proj = proj;
		vp.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		return vp;
	}
private:
	Screen& screen;
	glm::mat4 proj;
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	float speed = 30;
};