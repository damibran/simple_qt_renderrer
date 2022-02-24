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
			std::make_unique<MeshRenderer>(
				std::make_unique<LightCubeShader>()
				, std::make_unique<Mesh>("res/cub.obj")
				)
		));

		cub = std::shared_ptr<Shape>(new Shape(
			std::make_unique<MeshRenderer>(
				std::make_unique<CubeShader>(lightCube)
				, std::make_unique<Mesh>("res/cub.obj")
				)
		));

		lightCube->scale(glm::vec3(5));

		worldObj.addChild(cub);
		worldObj.addChild(lightCube);
	}

	void updateCamera(CameraAction ca)
	{
		cam.moveCamera(ca);
	}

	void updateScene(float dt,glm::vec3 cubScale)
	{
		//t += 0.7*dt;
		lightCube->rotate(speed*dt, { 0,1,0 });
		//cub->rotate(20*dt, { 0.2,-1,0.6 });
		cub->setScale(cubScale);
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
	float speed = 100;

};

#endif // SCENE_H
