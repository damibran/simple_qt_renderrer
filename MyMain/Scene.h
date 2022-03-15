#ifndef SCENE_H
#define SCENE_H
#include "Screen.h"
#include "Shape.h"
#include "../Shaders/ConcreteShaders/LightSourceShader.h"
#include "../Renderers/ConcreteRenderers/CoordSystemRenderer.h"
#include "../Scripts/ConcreteScripts/CameraScript.h"
#include "../Scripts/ConcreteScripts/BezierSurfaceScript.h"
#include "ui_RenderrerMainWindow.h"

class Scene
{
public:
	explicit Scene(Screen& s) :
		screen_(s)
	{
	}

	void setupScene(Ui::RenderrerMainWindowClass& ui)
	{
		std::unique_ptr<Transform> t(new Transform());
		t->scale({100, 100, 100});

		world_obj_.addChild(std::make_unique<Shape>(
			std::move(t),
			std::make_unique<CoordSystemRenderer>(screen_)
		));

		t = std::make_unique<Transform>();
		t->setPos({0, 20, 0});

		Transform* light_transform = world_obj_.addChildAndGetTransform(std::make_unique<Shape>(
			std::move(t),
			std::make_unique<ShaderMeshRenderer>(screen_, std::make_unique<LightSourceShader>(),
			                                     std::make_unique<Mesh>("res/cub.obj"))));

		world_obj_.addChild(BezierSurfaceScript::createObject(screen_, light_transform, 5, 5,10,10, "res/BiggerAsymmetricWavy5x5.obj"));

		cam_ = dynamic_cast<CameraScript*>(world_obj_.addChildAndGetScriptPtr(CameraScript::createObject(ui, screen_)));
	}

	void updateScene(float dt) const
	{
		world_obj_.updateScript(dt);
	}

	void renderScene() const
	{
		world_obj_.drawShape(screen_, cam_->getCameraProjViewMat());
	}

private:
	Screen& screen_;
	Shape world_obj_;
	////////
	CameraScript* cam_ = nullptr;
};

#endif // SCENE_H
