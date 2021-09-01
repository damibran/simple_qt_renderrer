#pragma once
#include"../utils/MVP_mat.h"
#include "../Screen.h"
#include "../shapes/Mesh.h"
#include "../utils/MeshRenderer.h"
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

//== 2. Библиотека фигур ==
class Shape
{ // Виртуальный базовый класс "фигура"

public:
	Shape(Shape&) = delete;
	Shape() = default;
	Shape(std::unique_ptr<MeshRenderer> mr) : mshRndr(std::move(mr)) {}

	void addChild(std::shared_ptr<Shape>& s)
	{
		childs.push_back(s);
	}
	
	void drawShape(Screen &screen,const MVP_mat& parent_trans)const//meant to only used with world obj
	{
		MVP_mat thisTrans(parent_trans);
		thisTrans.model = parent_trans.model * position * rotation * scaling;

		for (int i = 0; i < childs.size(); ++i)
		{
			childs[i]->drawChild(screen, thisTrans);
		}
	}

	void translate(const glm::vec3& v)
	{
		position = glm::translate(position, v);
	}
	void setPos(const glm::vec3& v)
	{
		position = glm::translate(glm::mat4(1), v);
	}
	void rotate(float angle, glm::vec3 v)
	{
		rotation = glm::rotate(rotation, glm::radians(angle), v);
	}
	void scale(const glm::vec3& factor)
	{
		scaling = glm::scale(scaling, factor);
	}
	glm::vec3 getPos()const
	{
		return position * glm::vec4(1);
	}
private:

	void drawChild(Screen &screen,const MVP_mat& parent_trans)const
	{
		MVP_mat thisTrans(parent_trans);
		thisTrans.model = parent_trans.model * position * rotation * scaling;

		if (mshRndr.get() != nullptr)
			mshRndr->drawMesh(screen,thisTrans);

		for (int i = 0; i < childs.size(); ++i)
		{
			childs[i]->drawChild(screen,thisTrans);
		}
	}

	std::vector<std::shared_ptr<Shape>> childs;
	std::unique_ptr<MeshRenderer> mshRndr;
	glm::mat4 position = glm::mat4(1.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::mat4 scaling = glm::mat4(1.0f);


};

