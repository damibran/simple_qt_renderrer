#pragma once
#include <memory>
#include "ui_RenderrerMainWindow.h"
#include "../Renderers/ConcreteRenderers/MeshClipShaderMeshRenderer.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShader.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShaderWithWireframe.h"
#include "../MyMain/Shape.h"
#include "../Script.h"
#include "../utils/Transform.h"

class MainShapeScript : public Script
{
public:
	static std::unique_ptr<Shape> createObject(Ui::RenderrerMainWindowClass& ui,
	                                           Screen& s,
	                                           const std::unordered_map<std::string, std::unique_ptr<Mesh>>&
	                                           mesh_instances, const std::string& main, const std::string& clip,
	                                           std::unique_ptr<Transform>& light,
	                                           std::unique_ptr<Transform>& clip_trans)
	{
		auto shp = std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3(0), glm::vec3(5)),
			std::make_unique<MeshClipShaderMeshRenderer>(
				s, std::make_unique<OnePointSourceLitShader>(light),
				mesh_instances.find(main)->second,
				mesh_instances.find(clip)->second, clip_trans, true));

		ui.MainTransformEditor->bindWidgetToShape(shp->getTransformPtr().get());

		return shp;
	}

	MainShapeScript() = default;

	void updateScript(float dt) override
	{
	}

private:
};
