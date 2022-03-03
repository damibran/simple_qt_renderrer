#ifndef SCENE_H
#define SCENE_H
#include "Screen.h"
#include "Shaders/ConcreteShaders/CubeShader.h"
#include "Shaders/ConcreteShaders/LightCubeShader.h"
#include "Renderers/CoordSystemRenderer.h"
#include "shapes/Mesh.h"
#include "shapes/shape.h"
#include "Renderers/BezierCurveRenderer.h"
#include "utils/camera.h"

class Scene
{
public:
	Scene(Screen& s, std::array<glm::vec3, 5>*& cntrlPts_ptr,bool& bezierPathNeedUpdate) :
		screen(s)
		, worldObj()
		, cam(s)
	{

		std::array<glm::vec3, 5> arr =
		{
			glm::vec3(0,0,0),
			glm::vec3(10,10,-10),
			glm::vec3(20,-20,-20),
			glm::vec3(30,30,-30),
			glm::vec3(10,10,0)
		};

		bezierCurve = std::make_shared<Shape>(
			std::make_unique<BezierCurveRenderer>(
				screen,
				cntrlPts_ptr,
				bezierPathNeedUpdate,
				6
				)
			);

		coordSys = std::make_shared<Shape>(
			std::make_unique<CoordSystemRenderer>(screen)
			);

		coordSys->scale({ 100,100,100 });

		worldObj.addChild(coordSys);
		worldObj.addChild(bezierCurve);
	}

	void updateCameraPos(float dt, glm::vec3 moveDir)
	{
		cam.moveCamera(moveDir, dt);
	}

	void updateCameraRot(glm::vec2 mouseDir)
	{
		cam.rotateCamera(mouseDir);
	}

	void updateScene(float dt)
	{
		t += 0.7 * dt;
	}

	void renderScene()
	{
		worldObj.drawShape(screen, cam.getCameraProjViewMat());
	}

	glm::vec3 getCamPosition()
	{
		return cam.cameraPos;
	}

private:
	Screen& screen;
	Shape worldObj;
	Camera cam;
	////////
	std::shared_ptr<Shape> bezierCurve;
	std::shared_ptr<Shape> coordSys;
	float t = 0;
};

#endif // SCENE_H
