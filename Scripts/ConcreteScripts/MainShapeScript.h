#pragma once
#include <memory>
#include "ui_RenderrerMainWindow.h"
#include "../Renderers/ConcreteRenderers/MeshClipShaderMeshRenderer.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShader.h"
#include "../Shaders/ConcreteShaders/UnlitTexturedRepeatShader.h"
#include "../Shaders/ConcreteShaders/LitTexturedShader.h"
#include "../Shaders/ConcreteShaders/FunShader.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShaderWithWireframe.h"
#include "../MyMain/Shape.h"
#include "../Script.h"
#include "../utils/Transform.h"
#include "../utils/MeshInstances.hpp"

class MainShapeScript : public Script
{
public:
	static std::unique_ptr<Shape> createObject(Ui::RenderrerMainWindowClass& ui,
	                                           Screen& s,
	                                           const MeshInstances&
	                                           mesh_instances, const std::string& main,
	                                           std::unique_ptr<Transform>& light)
	{
		auto shp = std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3(0), glm::vec3(5)),
			std::make_unique<ShaderMeshRenderer>(
				s, std::make_unique<LitTexturedShader>(light),
				mesh_instances.get(main)));

		ui.MainTransformEditor->bindWidgetToShape(shp->getTransformPtr().get());

		return shp;
	}

	MainShapeScript() = default;

	void updateScript(float dt) override
	{
	}

private:
};
