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
	                                           mesh_instances, const std::string& main,
	                                           std::unique_ptr<Transform>& light)
	{
		auto shp = std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3(0), glm::vec3(5)),
			std::make_unique<ShaderMeshRenderer>(
				s, std::make_unique<OnePointSourceLitShaderWithWireframe>(light),
				mesh_instances.at(main)));

		ui.MainTransformEditor->bindWidgetToShape(shp->getTransformPtr().get());

		return shp;
	}

	MainShapeScript() = default;

	void updateScript(float dt) override
	{
	}

private:
};
