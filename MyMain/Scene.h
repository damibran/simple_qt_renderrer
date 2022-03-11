#ifndef SCENE_H
#define SCENE_H
#include "Screen.h"
#include "Shape.h"
#include "../Renderers/ConcreteRenderers/BezierCurveRenderer.h"
#include "../Renderers/ConcreteRenderers/CoordSystemRenderer.h"
#include "../Scripts/ConcreteScripts/BezierScript.h"
#include "../Scripts/ConcreteScripts/CameraScript.h"
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
		t->scale({ 100,100,100 });

		world_obj_.addChild(std::make_unique<Shape>(
			std::move(t),
			std::make_unique<CoordSystemRenderer>(screen_)
			));

		world_obj_.addChild(BezierScript::createShape(ui, screen_, 6));
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
	CameraScript* cam_=nullptr;
};

#endif // SCENE_H
