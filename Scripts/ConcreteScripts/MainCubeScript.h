#pragma once
#include <memory>

#include "../Renderers/ConcreteRenderers/MeshClipShaderMeshRenderer.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShader.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShaderWithWireframe.h"
#include "../MyMain/Shape.h"
#include "../Script.h"
#include "../utils/Transform.h"

class MainCubeScript : public Script
{
public:
	static std::unique_ptr<Shape> createObject(Screen& s, Transform* light, Transform* clip_trans)
	{
		auto shp = std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3(0), glm::vec3(10)),
			std::make_unique<MeshClipShaderMeshRenderer>(
				s, std::make_unique<OnePointSourceLitShaderWithWireframe>(light),
				std::make_unique<Mesh>("res/cub.obj"),
				std::make_unique<Mesh>("res/cub.obj"), clip_trans));

		shp->setScript(std::make_unique<MainCubeScript>(shp->getTransformPtr()));
		return shp;
	}

	MainCubeScript(Transform* trans): transform_(trans)
	{
	}

	void updateScript(float dt) override
	{
		angle += 1 * dt;
		transform_->setPos({10*sin(angle), 10*cos(angle), 10*sin(angle)});
	}

private:
	float angle = 0;
	Transform* transform_;
};
