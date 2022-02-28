#ifndef SCENE_H
#define SCENE_H
#include "Screen.h"
#include "Shaders/ConcreteShaders/CubeShader.h"
#include "Shaders/ConcreteShaders/LightCubeShader.h"
#include "Shaders/ConcreteShaders/WireframeShader.h"
#include "shapes/Mesh.h"
#include "shapes/shape.h"
#include "utils/camera.h"

class Scene
{
public:
	Scene(Screen& s) :
		screen(s)
		, worldObj()
		, cam(s)
	{

		lightCube = std::shared_ptr<Shape>(new Shape(
			std::make_unique<ShaderMeshRenderer>(
				screen,
				std::make_unique<LightCubeShader>(),
				std::make_unique<Mesh>("res/cub.obj")
				)
		));

		cub = std::shared_ptr<Shape>(new Shape(
			std::make_unique<ShaderMeshRenderer>(
				screen,
				std::make_unique<CubeShader>(lightCube),
				std::make_unique<Mesh>("res/cub.obj")
				)
		));

		//lightCube->scale(glm::vec3(5));
		cub->scale({ 10,10,10 });

		worldObj.addChild(cub);
		worldObj.addChild(lightCube);
	}

	void updateCamera(CameraAction ca)
	{
		cam.moveCamera(ca);
	}

	void updateScene(float dt)
	{
		t += 0.7 * dt;
		lightCube->setPos({ 30 * cos(t),0,30 * sin(t) });

		cub->rotate(20 * dt, { 0.2,-1,0.6 });
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
	std::shared_ptr<Shape> cub;
	std::shared_ptr<Shape> lightCube;
	float t = 0;

};

#endif // SCENE_H
