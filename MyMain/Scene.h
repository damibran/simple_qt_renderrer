#ifndef SCENE_H
#define SCENE_H
#include "Screen.h"
#include "Shape.h"
#include "../Shaders/ConcreteShaders/LightSourceShader.h"
#include "../Scripts/ConcreteScripts/MainShapeScript.h"
#include "../Renderers/ConcreteRenderers/CoordSystemRenderer.h"
#include "../Shaders/ConcreteShaders/WireframeShader.h"
#include "../Scripts/ConcreteScripts/CameraScript.h"
#include "../Scripts/ConcreteScripts/PointLightSourceScript.h"
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
		mesh_instances_["res/cub.obj"] = std::make_unique<Mesh>("res/cub.obj");
		mesh_instances_["res/tetrahedron.obj"] = std::make_unique<Mesh>("res/tetrahedron.obj");
		mesh_instances_["res/monkey.obj"] = std::make_unique<Mesh>("res/monkey.obj");
		mesh_instances_["res/tree.obj"] = std::make_unique<Mesh>("res/tree.obj");
		mesh_instances_["res/pyramid.obj"] = std::make_unique<Mesh>("res/pyramid.obj");

		// Make CoordSys shape
		scene_root_.push_back(std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3(0), glm::vec3(100)),
			std::make_unique<CoordSystemRenderer>(screen_)
		));

		// Make Light source shape
		scene_root_.push_back(PointLightSourceScript::createObject(ui, screen_, mesh_instances_));
		std::unique_ptr<Transform>& light_transform = (scene_root_.end() - 1)->get()->getTransformPtr();

		// Make main cube
		scene_root_.push_back(
			MainShapeScript::createObject(ui, screen_, mesh_instances_, "res/monkey.obj",
			                              light_transform));

		//Make camera shape
		scene_root_.push_back(CameraScript::createObject(ui, screen_));
		cam_ = dynamic_cast<CameraScript*>((scene_root_.end() - 1)->get()->getScriptPtr());
	}

	void updateScene(float dt) const
	{
		for (auto& i : scene_root_)
			i->updateScript(dt);
	}

	void renderScene()const
	{
		std::chrono::system_clock::time_point tp1_ = std::chrono::system_clock::now();

		screen_.pool_.paused=true;

		for (auto& i : scene_root_)
			i->drawShape(screen_, cam_->getCameraProjViewMat());

		screen_.pool_.paused=false;
		screen_.pool_.wait_for_tasks();

		std::chrono::system_clock::time_point tp2_ = std::chrono::system_clock::now();
		const std::chrono::duration<float> elapsed_time = tp2_ - tp1_;
		qDebug()<<1./elapsed_time.count();

		screen_.sumUpBuffers();
	}

private:
	std::chrono::system_clock::time_point tp1_ = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point tp2_ = std::chrono::system_clock::now();
	Screen& screen_;
	std::vector<std::unique_ptr<Shape>> scene_root_;
	std::unordered_map<std::string, std::unique_ptr<Mesh>> mesh_instances_;
	////////
	CameraScript* cam_ = nullptr;
};

#endif // SCENE_H
