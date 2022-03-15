#pragma once
#include <memory>
#include "../Shaders/ConcreteShaders/OneSourceLitShader.h"
#include "../Renderers/ConcreteRenderers/MeshRenderer.h"
#include "../MyMain/Shape.h"
#include "../Script.h"

class BezierSurfaceScript final : public Script
{
public:

	std::unique_ptr<Shape> static createObject(Screen& s, Transform* light_source_transform, int patch_u,int patch_v ,const std::string& controlMeshPath)
	{
		std::unique_ptr<Transform> t(new Transform);
		t->translate({ 0,0,0 });
		auto shader = std::make_unique<OneSourceLitShader>(light_source_transform);
		auto renderer = std::make_unique<ShaderMeshRenderer>(s, std::move(shader), Mesh::generateBezierSurface(controlMeshPath, patch_u, patch_v));

		return std::make_unique<Shape>(std::move(t),std::move(renderer));
	}

	void updateScript(float dt) override
	{
		
	}
};