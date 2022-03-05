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
		, cam(std::make_unique<Camera>(s))
	{

		cam->translate({ 0,0,60 });

		bezierCurve = std::make_shared<Shape>(
			std::make_shared<BezierCurveRenderer>(
				screen,
				cntrlPts_ptr,
				bezierPathNeedUpdate,
				6
				)
			);

		std::shared_ptr<Shape> coordSys = std::make_shared<Shape>(
			std::make_unique<CoordSystemRenderer>(screen)
			);

		coordSys->scale({ 100,100,100 });

		worldObj.addChild(coordSys);
		worldObj.addChild(bezierCurve);
	}

	void updateCameraPos(float dt, glm::vec3 moveDir)
	{
		cam->moveCamera(moveDir, dt);
	}

	void updateCameraRot(glm::vec2 mouseDir)
	{
		cam->rotateCamera(mouseDir);
	}

	void setCurveRotation(float xRot, float yRot)
	{
		bezierCurve->setRotationDegrees(glm::vec3(xRot,yRot,0.));
	}

	void updateScene(float dt)
	{
		t += 0.7 * dt;
	}

	void renderScene()
	{
		worldObj.drawShape(screen, cam->getCameraProjViewMat());
	}

private:
	Screen& screen;
	Shape worldObj;
	//Camera cam;
	////////
	std::shared_ptr<Camera> cam;
	std::shared_ptr<Shape> bezierCurve;
	float t = 0;
};

#endif // SCENE_H
