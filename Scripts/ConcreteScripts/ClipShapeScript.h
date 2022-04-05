#pragma once
#include <memory>
#include "ui_RenderrerMainWindow.h"
#include "../Renderers/ConcreteRenderers/MeshClipShaderMeshRenderer.h"
#include "../Shaders/ConcreteShaders/WireframeShader.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShaderWithWireframe.h"
#include "../MyMain/Shape.h"
#include "../Script.h"
#include "../utils/Transform.h"

class ClipShapeScript : public Script
{
public:
	static std::unique_ptr<Shape> createObject(Ui::RenderrerMainWindowClass& ui,
	                                           Screen& s,
	                                           const std::unordered_map<std::string, std::unique_ptr<Mesh>>&
	                                           mesh_instances, const std::string& mesh)
	{
		auto shp = std::make_unique<Shape>(std::make_unique<Transform>(glm::vec3(0), glm::vec3(15)),
		                                   std::make_unique<ShaderMeshRenderer>(
			                                   s, std::make_unique<WireFrameShader>(0.0003),
			                                   mesh_instances.find(mesh)->second
		                                   )
		);

		ui.ClipTransformEditor->bindWidgetToShape(shp->getTransformPtr().get());

		return shp;
	}

	ClipShapeScript() = default;

	void updateScript(float dt) override
	{
	}

private:
};
