#ifndef SCENE_H
#define SCENE_H
#include "Screen.h"
#include "Renderers/CoordSystemRenderer.h"
#include "shapes/Mesh.h"
#include "shapes/shape.h"
#include "Renderers/BezierCurveRenderer.h"
#include "utils/camera.h"

class Scene
{
public:
	Scene(Screen& s, std::array<glm::vec3, 5>*& cntrl_pts_ptr,bool& bezier_path_need_update) :
		screen_(s)
		, world_obj_()
		, cam_(std::make_unique<Camera>(s))
	{

		cam_->translate({ 0,0,60 });

		bezier_curve_ = std::make_shared<Shape>(
			std::make_shared<BezierCurveRenderer>(
				screen_,
				cntrl_pts_ptr,
				bezier_path_need_update,
				6
				)
			);

		const std::shared_ptr<Shape> coordSys = std::make_shared<Shape>(
			std::make_unique<CoordSystemRenderer>(screen_)
			);

		coordSys->scale({ 100,100,100 });

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

	void setCurveRotation(const float x_rot, const float y_rot) const
	{
		bezier_curve_->setRotationDegrees(glm::vec3(x_rot,y_rot,0.));
	}

	void updateScene(float dt)
	{
		t_ += dt * 0.7;
	}

	void renderScene() const
	{
		world_obj_.drawShape(screen_, cam_->getCameraProjViewMat());
	}

private:
	Screen& screen_;
	Shape world_obj_;
	//Camera cam;
	////////
	std::shared_ptr<Camera> cam_;
	std::shared_ptr<Shape> bezier_curve_;
	float t_ = 0;
};

#endif // SCENE_H
