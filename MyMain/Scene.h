#ifndef SCENE_H
#define SCENE_H
#include "Screen.h"
#include "Shape.h"
#include "../Shaders/ConcreteShaders/LightSourceShader.h"
#include "../Scripts/ConcreteScripts/MainCubeScript.h"
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
		// Make CoordSys shape
		scene_root_.push_back(std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3(0), glm::vec3(100)),
			std::make_unique<CoordSystemRenderer>(screen_)
		));

		// Make Light source shape
		scene_root_.push_back(PointLightSourceScript::createObject(ui, screen_));
		Transform* light_transform = (scene_root_.end() - 1)->get()->getTransformPtr();

		scene_root_.push_back(std::make_unique<Shape>(std::make_unique<Transform>(glm::vec3(0), glm::vec3(15)),
		                                              std::make_unique<ShaderMeshRenderer>(
			                                              screen_, std::make_unique<WireFrameShader>(0.0003),
			                                              std::make_unique<Mesh>("res/cub.obj"))));

		// Make main cube
		scene_root_.push_back(MainCubeScript::createObject(screen_, light_transform));

		//Make camera shape
		scene_root_.push_back(CameraScript::createObject(ui, screen_));
		cam_ = dynamic_cast<CameraScript*>((scene_root_.end() - 1)->get()->getScriptPtr());
	}

	void updateScene(float dt) const
	{
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
	////////
	CameraScript* cam_ = nullptr;
};

#endif // SCENE_H
