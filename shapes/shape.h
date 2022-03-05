#pragma once
#include"../utils/MVP_mat.h"
#include "../Screen.h"
#include "../shapes/Mesh.h"
#include "../Renderers/RendererComponent.h"
#include "../Renderers/MeshRenderer.h"
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Shape
{

public:
	Shape(Shape&) = delete;
	Shape() = default;
	Shape(std::shared_ptr<RendererComponent> mr) : rndr(mr) {}

	void addChild(std::shared_ptr<Shape>& s)
	{
		childs.push_back(s);
	}

	void drawShape(Screen& screen, const MVP_mat& parent_trans)const //meant to be used only with world obj
	{
		MVP_mat thisTrans(parent_trans);
		thisTrans.model = parent_trans.model * glm::translate(glm::mat4(1), position) * getRotationMatrix() * glm::scale(glm::mat4(1), scaling);

		for (int i = 0; i < childs.size(); ++i)
		{
			childs[i]->drawChild(screen, thisTrans);
		}
	}

	void translate(const glm::vec3& v)
	{
		position = glm::translate(glm::mat4(1), v) * glm::vec4(position, 1);
	}
	void setPos(const glm::vec3& v)
	{
		position = v;
	}
	glm::vec3 getPos()const
	{
		return position;
	}

	void setRotationDegrees(const glm::vec3 v)
	{
		rotation = v;
	}

	void scale(const glm::vec3& factor)
	{
		scaling *= factor;
	}
	void setScale(const glm::vec3& v)
	{
		scaling = v;
	}

protected:

	void drawChild(Screen& screen, const MVP_mat& parent_trans)const
	{
		MVP_mat thisTrans(parent_trans);
		thisTrans.model = parent_trans.model * glm::translate(glm::mat4(1), position) * getRotationMatrix() * glm::scale(glm::mat4(1), scaling);

		if (rndr.get() != nullptr)
			rndr->drawShapeVisual(thisTrans);

		for (int i = 0; i < childs.size(); ++i)
		{
			childs[i]->drawChild(screen, thisTrans);
		}
	}

	glm::mat4 getRotationMatrix()const
	{
		glm::mat4 m(1.);
		m = glm::rotate(m, glm::radians(rotation.x), { 1,0,0 });
		m = glm::rotate(m, glm::radians(rotation.y), { 0,1,0 });
		m = glm::rotate(m, glm::radians(rotation.z), { 0,0,1 });
		return m;
	}

	std::vector<std::shared_ptr<Shape>> childs;
	std::shared_ptr<RendererComponent> rndr;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.f); // pitch, yaw, roll in degrees
	glm::vec3 scaling = glm::vec3(1.0f);
};

