#pragma once
#include"../utils/MVPMat.h"
#include "Screen.h"
#include "../Renderers/RendererComponent.h"
#include "../Scripts/Script.h"
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../utils/Transform.h"

class Shape
{
public:
	Shape(Shape&) = delete;

	explicit Shape(std::unique_ptr<Transform> transform = nullptr,
	               std::unique_ptr<RendererComponent> mr = nullptr,
		std::unique_ptr<Script> script = nullptr

	) : transform_(std::move(transform)),
	    renderer_(std::move(mr)),
	    script_(std::move(script))
	{
		if (!transform_)
			transform_ = std::make_unique<Transform>();
	}

	void addChild(std::unique_ptr<Shape> s)
	{
		childs_.push_back(std::move(s));
	}

	Script* addChildAndGetScriptPtr(std::unique_ptr<Shape> s)
	{
		childs_.push_back(std::move(s));
		return (childs_.end() - 1)->get()->getScriptPtr();
	}

	void drawShape(Screen& screen, const MVPMat& parent_trans) const
	{
		MVPMat this_trans(parent_trans);
		this_trans.model = parent_trans.model * transform_->getFullModelMatrix();

		if (renderer_)
			renderer_->drawShapeVisual(this_trans);

		for (auto& i : childs_)
		{
			i->drawShape(screen, this_trans);
		}
	}

	void updateScript(float dt) const
	{
		if (script_)
			script_->updateScript(dt);

		for (auto& i : childs_)
			i->updateScript(dt);
	}

	void setScript(std::unique_ptr<Script> s)
	{
		script_ = std::move(s);
	}

	Script* getScriptPtr()
	{
		return script_.get();
	}

	Transform* getTransformPtr()
	{
		return transform_.get();
	}

	RendererComponent* getRenderComponent()
	{
		return renderer_.get();
	}

protected:
	std::vector<std::unique_ptr<Shape>> childs_;
	std::unique_ptr<Transform> transform_;
	std::unique_ptr<RendererComponent> renderer_;
	std::unique_ptr<Script> script_;
};
