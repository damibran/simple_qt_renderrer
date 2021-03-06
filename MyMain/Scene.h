#ifndef SCENE_H
#define SCENE_H
#include "Screen.h"
#include "Shape.h"
#include "../Shaders/ConcreteShaders/LightSourceShader.h"
#include "../Shaders/ConcreteShaders/LitTexturedRepeatShader.h"
#include "../Scripts/ConcreteScripts/MainShapeScript.h"
#include "../Shaders/ConcreteShaders/LitScanerShader.h"
#include "../Renderers/ConcreteRenderers/CoordSystemRenderer.h"
#include "../Renderers/ConcreteRenderers/ClipNearShaderMeshRenderer.h"
#include "../Shaders/ConcreteShaders/WireframeShader.h"
#include "../Scripts/ConcreteScripts/CameraScript.h"
#include "../Scripts/ConcreteScripts/PointLightSourceScript.h"
#include "../utils/MeshInstances.hpp"
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
		mesh_instances_.add("res/texCub.obj");
		mesh_instances_.add("res/barell.obj");
		mesh_instances_.add("res/grassPlane.obj");
		mesh_instances_.add("res/tetrahedron.obj");

		// Make CoordSys shape
		//scene_root_.push_back(std::make_unique<Shape>(
		//	std::make_unique<Transform>(glm::vec3{0, -4, 0}, glm::vec3(100)),
		//	std::make_unique<CoordSystemRenderer>(screen_)
		//));

		// Make Light source shape
		scene_root_.push_back(PointLightSourceScript::createObject(ui, screen_, mesh_instances_));
		std::unique_ptr<Transform>& light_transform = (scene_root_.end() - 1)->get()->getTransformPtr();

		// Make main shape
		scene_root_.push_back(
			MainShapeScript::createObject(ui, screen_, mesh_instances_, "res/barell.obj",
			                              light_transform));

		// Make fun shape
		scene_root_.push_back(
			std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3{-40,0,0}, glm::vec3(5)),
			std::make_unique<ShaderMeshRenderer>(
				screen_, std::make_unique<FunShader>(time_),
				mesh_instances_.get("res/barell.obj")))
		);

		// Make scaner shape
		scene_root_.push_back(std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3{40, 0, 0}, glm::vec3(5)),
			std::make_unique<ShaderMeshRenderer>(
				screen_, std::make_unique<LitScanerShader>(light_transform),
				mesh_instances_.get("res/barell.obj")))
			);

		// Make plane shape
		scene_root_.push_back(std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3{0, -3, 0}, glm::vec3(100,1,100)),
			std::make_unique<ClipNearShaderRenderer>(
				screen_, std::make_unique<LitTexturedRepeatShader>(15,light_transform),
				mesh_instances_.get("res/grassPlane.obj")))
			);

		//Make camera shape
		scene_root_.push_back(CameraScript::createObject(ui, screen_));
		cam_ = dynamic_cast<CameraScript*>((scene_root_.end() - 1)->get()->getScriptPtr());
	}

	void updateScene(float dt)
	{
		const std::chrono::duration<double> elapsed_time = std::chrono::system_clock::now() - start_;
		time_ = elapsed_time.count();
		for (auto& i : scene_root_)
			i->updateScript(dt);
	}

	void renderScene() const
	{
		for (auto& i : scene_root_)
			i->drawShape(screen_, cam_->getCameraProjViewMat());
	}

private:
	Screen& screen_;
	std::vector<std::unique_ptr<Shape>> scene_root_;
	MeshInstances mesh_instances_;
	////////
	CameraScript* cam_ = nullptr;
	std::chrono::system_clock::time_point start_ = std::chrono::system_clock::now();
	double time_ = 0;
};

#endif // SCENE_H
