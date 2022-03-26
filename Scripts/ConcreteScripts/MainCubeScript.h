#pragma once
#include <memory>

#include "../Renderers/ConcreteRenderers/ShaderMeshRenderer.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShader.h"
#include "../MyMain/Shape.h"
#include "../Script.h"
#include "../utils/Transform.h"

class MainCubeScript : public Script
{
public:

	static std::unique_ptr<Shape> createObject(Screen& s,Transform* light)
	{
		auto shp = std::make_unique<Shape>(
			std::make_unique<Transform>(glm::vec3(0),glm::vec3(10)),
			std::make_unique<ShaderMeshRenderer>(s, std::make_unique<OnePointSourceLitShader>(light),
			                                     std::make_unique<Mesh>("res/cub.obj")));

		shp->setScript(std::make_unique<MainCubeScript>(shp->getTransformPtr()));
		return shp;
	}

	MainCubeScript(Transform* trans):transform_(trans){}

	void updateScript(float dt) override
	{
		transform_->setRotationDegrees({angle+=10*dt,angle,0});
	}
private:
	float angle=0;
	Transform* transform_;
};
