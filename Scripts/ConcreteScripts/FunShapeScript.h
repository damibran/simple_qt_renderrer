#pragma once
#include <memory>
#include "ui_RenderrerMainWindow.h"
#include "../Renderers/ConcreteRenderers/MeshClipShaderMeshRenderer.h"
#include "../Shaders/ConcreteShaders/FunShader.h"
#include "../MyMain/Shape.h"
#include "../Script.h"
#include "../utils/Transform.h"
#include "../utils/MeshInstances.hpp"

class FunShapeScript : public Script
{
public:
	static std::unique_ptr<Shape> createObject(Screen& s,
	                                           const MeshInstances&
	                                           mesh_instances, const std::string& main,
	                                           double& time)
	{
		auto shp = std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3{-40,0,0}, glm::vec3(5)),
			std::make_unique<ShaderMeshRenderer>(
				s, std::make_unique<FunShader>(time),
				mesh_instances.get(main)));


		return shp;
	}

	FunShapeScript() = default;

	void updateScript(float dt) override
	{

	}

private:

};
