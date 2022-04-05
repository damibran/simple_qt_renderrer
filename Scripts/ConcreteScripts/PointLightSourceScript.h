#pragma once
#include <QCheckBox>
#include <memory>
#include <ui_RenderrerMainWindow.h>
#include "../utils/Transform.h"
#include "../Shaders/ConcreteShaders/LightSourceShader.h"
#include "../Renderers/ConcreteRenderers/ShaderMeshRenderer.h"
#include "../MyMain/Shape.h"
#include "../Script.h"

class PointLightSourceScript : public Script
{
public:
	PointLightSourceScript(std::unique_ptr<Transform>& t, QCheckBox* qcb): transform_(t), check_box_(qcb)
	{
	}

	static std::unique_ptr<Shape> createObject(Ui::RenderrerMainWindowClass& ui, Screen& s,
	                                           const std::unordered_map<std::string, std::unique_ptr<Mesh>>&
	                                           mesh_instances)
	{
		auto shp = std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3(0, 30, 0)),
			std::make_unique<ShaderMeshRenderer>(s, std::make_unique<LightSourceShader>(),
			                                     mesh_instances.find("res/cub.obj")->second));

		shp->setScript(std::make_unique<PointLightSourceScript>(shp->getTransformPtr(), ui.checkBox));

		return shp;
	}

	void updateScript(float dt) override
	{
		if (check_box_->isChecked())
		{
			t_ += speed_ * dt;
			transform_->setPos({20 * sin(t_), 30 * cos(t_), 0});
		}
	}

private:
	float speed_ = 1;
	float t_ = 0.;
	std::unique_ptr<Transform>& transform_;
	QCheckBox* check_box_;
};
