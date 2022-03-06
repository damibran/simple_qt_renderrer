#pragma once
#include"../utils/MVPMat.h"
#include "../Screen.h"
#include "../Renderers/RendererComponent.h"
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Shape
{

public:
	Shape(Shape&) = delete;
	Shape() = default;
	explicit Shape(const std::shared_ptr<RendererComponent>& mr) : rndr_(mr) {}

	void addChild(const std::shared_ptr<Shape>& s)
	{
		childs_.push_back(s);
	}

	void drawShape(Screen& screen, const MVPMat& parent_trans)const //meant to be used only with world obj
	{
		MVPMat this_trans(parent_trans);
		this_trans.model = parent_trans.model * glm::translate(glm::mat4(1), position_) * getRotationMatrix() * glm::scale(glm::mat4(1), scaling_);

		for (auto &i : childs_)
		{
			i->drawChild(screen, this_trans);
		}
	}

	void translate(const glm::vec3& v)
	{
		position_ = glm::translate(glm::mat4(1), v) * glm::vec4(position_, 1);
	}
	void setPos(const glm::vec3& v)
	{
		position_ = v;
	}

	[[nodiscard]] glm::vec3 getPos()const
	{
		return position_;
	}

	void setRotationDegrees(const glm::vec3 v)
	{
		rotation_ = v;
	}

	void scale(const glm::vec3& factor)
	{
		scaling_ *= factor;
	}
	void setScale(const glm::vec3& v)
	{
		scaling_ = v;
	}

protected:

	void drawChild(Screen& screen, const MVPMat& parent_trans)const
	{
		MVPMat thisTrans(parent_trans);
		thisTrans.model = parent_trans.model * glm::translate(glm::mat4(1), position_) * getRotationMatrix() * glm::scale(glm::mat4(1), scaling_);

		if (rndr_ != nullptr)
			rndr_->drawShapeVisual(thisTrans);

		for (auto& i : childs_)
		{
			i->drawChild(screen, thisTrans);
		}
	}

	[[nodiscard]] glm::mat4 getRotationMatrix()const
	{
		glm::mat4 m(1.);
		m = glm::rotate(m, glm::radians(rotation_.x), { 1,0,0 });
		m = glm::rotate(m, glm::radians(rotation_.y), { 0,1,0 });
		m = glm::rotate(m, glm::radians(rotation_.z), { 0,0,1 });
		return m;
	}

	std::vector<std::shared_ptr<Shape>> childs_;
	std::shared_ptr<RendererComponent> rndr_;
	glm::vec3 position_ = glm::vec3(0.0f);
	glm::vec3 rotation_ = glm::vec3(0.f); // pitch, yaw, roll in degrees
	glm::vec3 scaling_ = glm::vec3(1.0f);
};

