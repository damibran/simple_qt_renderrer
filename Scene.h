#ifndef SCENE_H
#define SCENE_H
#include "Screen.h"
#include "Shaders/ConcreteShaders/CubeShader.h"
#include "shapes/Mesh.h"
#include "shapes/shape.h"
#include "utils/camera.h"

class Scene
{
public:
    Scene(Screen& s):
      screen(s)
      ,cubShdr()
      ,worldObj()
      ,cam(s)
    {
        cub=std::shared_ptr<Shape>(new Shape(
                std::make_unique<MeshRenderer>(
                    cubShdr,
                    std::make_unique<Mesh>("res/cub.obj")
                )
            ));
        lightSource = std::make_shared<Shape>();

        cub->scale({ 10,10,10 });
        cub->rotate(90, { 1,0,0 });

        lightSource->translate({ 0,0,30 });

        worldObj.addChild(cub);
        worldObj.addChild(lightSource);
    }

    void updateCamera(CameraAction ca)
    {
        cam.moveCamera(ca);
    }

    void updateScene()
    {
        t += 0.01;
        lightSource->setPos({30*cos(t),0,30*sin(t)});
        *(cubShdr.world_light_pos) = lightSource->getPos();
        cub->rotate(0.3, { 0.2,-1,0.6 });

        worldObj.drawShape(screen,cam.getCameraProjViewMat());
    }
private:
    Screen& screen;
    CubeShader cubShdr;
    Shape worldObj;
    Camera cam;
    ////////
    std::shared_ptr<Shape> cub;
    std::shared_ptr<Shape> lightSource;
    float t=0;

};

#endif // SCENE_H
