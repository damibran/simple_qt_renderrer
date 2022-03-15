#pragma once
#include <memory>
#include "../Shaders/ConcreteShaders/OneSourceLitShader.h"
#include "../Renderers/ConcreteRenderers/MeshRenderer.h"
#include "../MyMain/Shape.h"
#include "../Script.h"

class BezierSurfaceScript final : public Script
{
public:

	std::unique_ptr<Shape> static createObject(Screen& s, Transform* light_source_transform, const int patch_u, const int patch_v , const int res_u, const int res_v,const std::string& control_mesh_path)
	{
		std::unique_ptr<Transform> t(new Transform);
		t->translate({ 0,0,0 });
		auto shader = std::make_unique<OneSourceLitShader>(light_source_transform);
		auto renderer = std::make_unique<ShaderMeshRenderer>(s, std::move(shader), Mesh::generateBezierSurface(control_mesh_path, patch_u, patch_v,res_u,res_v));

		return std::make_unique<Shape>(std::move(t),std::move(renderer));
	}

	void updateScript(float dt) override
	{
		
	}
};