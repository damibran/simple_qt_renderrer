#ifndef SCENE_H
#define SCENE_H
#include "Screen.h"
#include "Shape.h"
#include "../Renderers/ConcreteRenderers/BezierCurveRenderer.h"
#include "../Renderers/ConcreteRenderers/CoordSystemRenderer.h"
#include "../utils/Camera.h"
#include "../Scripts/ConcreteScripts/BezierScript.h"
#include "ui_RenderrerMainWindow.h"

class Scene
{
public:
	explicit Scene(Screen& s) :
		screen_(s)
		, cam_(std::make_unique<Camera>(s))
	{
	}

	void setupScene(Ui::RenderrerMainWindowClass& ui)
	{
		cam_->translate({0, 0, 60});

		auto bcr = std::make_shared<BezierCurveRenderer>(
			screen_, 6);

		bezier_curve_ = std::make_shared<Shape>(
			bcr);

		bezier_curve_->setScript(std::make_unique<BezierScript>(ui, bcr.get(),bezier_curve_.get()));

		const auto coordSys = std::make_shared<Shape>(
			std::make_unique<CoordSystemRenderer>(screen_)
		);

		coordSys->scale({100, 100, 100});

		world_obj_.addChild(coordSys);
		world_obj_.addChild(bezier_curve_);
	}

	void updateCameraPos(const float dt, const glm::vec3 move_dir) const
	{
		cam_->moveCamera(move_dir, dt);
	}

	void updateCameraRot(const glm::vec2 mouse_dir) const
	{
		cam_->rotateCamera(mouse_dir);
	}

	void updateScene(float dt)
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
	std::shared_ptr<Camera> cam_;
	std::shared_ptr<Shape> bezier_curve_;
};

#endif // SCENE_H
