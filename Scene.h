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

		cub = std::shared_ptr<Shape>(new Shape(
			std::make_unique<MeshRenderer>(
				std::make_unique<WireFrameShader>(0.00015)
				, std::make_unique<Mesh>("res/cub.obj")
				)
		));

		coordSys = std::shared_ptr<Shape>(new Shape(
			std::make_unique<MeshRenderer>(
				std::make_unique<WireFrameShader>(0.00002)
				, std::make_unique<Mesh>("res/CoordSys.obj")
				)
		));

		coordSys->scale({ 100,100,100 });

		worldObj.addChild(cub);
		worldObj.addChild(coordSys);
	}

	void updateCamera(CameraAction ca)
	{
		cam.moveCamera(ca);
	}

	void updateScene(float dt,glm::vec3 cubScale)
	{
		t += 0.7*dt;
		//cub->rotate(20*dt, { 0.2,-1,0.6 });
		cub->setScale(cubScale);
		worldObj.drawShape(screen, cam.getCameraProjViewMat());
	}
private:
	Screen& screen;
	Shape worldObj;
	Camera cam;
	////////
	std::shared_ptr<Shape> cub;
	std::shared_ptr<Shape> coordSys;
	float t = 0;

};

#endif // SCENE_H
