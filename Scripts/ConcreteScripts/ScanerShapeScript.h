#pragma once
#include <memory>
#include "ui_RenderrerMainWindow.h"
#include "../Renderers/ConcreteRenderers/MeshClipShaderMeshRenderer.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShader.h"
#include "../Shaders/ConcreteShaders/UnlitTexturedShader.h"
#include "../Shaders/ConcreteShaders/ScanerShader.h"
#include "../Shaders/ConcreteShaders/LitTexturedShader.h"
#include "../Shaders/ConcreteShaders/FunShader.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShaderWithWireframe.h"
#include "../MyMain/Shape.h"
#include "../Script.h"
#include "../utils/Transform.h"
#include "../utils/MeshInstances.hpp"

class ScanerShapeScript : public Script
{
public:
	static std::unique_ptr<Shape> createObject(Screen& s,
	                                           const MeshInstances&
	                                           mesh_instances, const std::string& main)
	{
		auto shp = std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3{40, 0, 0}, glm::vec3(5)),
			std::make_unique<ShaderMeshRenderer>(
				s, std::make_unique<ScanerShader>(),
				mesh_instances.get(main)));


		return shp;
	}

	ScanerShapeScript() = default;

	void updateScript(float dt) override
	{
	}

private:
};
