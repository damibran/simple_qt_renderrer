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
		proj = glm::perspective(glm::radians(90.0f), (float)screen.XMAX / (float)screen.YMAX, 0.1f, 200.0f);
	}
	void moveCamera(Input::CameraAction act, float dt)
	{
		float tspeed = dt * speed;
		if (act == Input::CameraAction::UP)
			cameraPos += tspeed * cameraUp;
		else if (act == Input::CameraAction::DOWN)
			cameraPos -= tspeed * cameraUp;
		else if (act == Input::CameraAction::LEFT)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * tspeed;
		else if (act == Input::CameraAction::RIGHT)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * tspeed;
		else if (act == Input::CameraAction::ZOOMOUT)
			cameraPos -= tspeed * cameraFront;
		else if (act == Input::CameraAction::ZOOMIN)
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