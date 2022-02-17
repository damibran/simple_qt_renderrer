#pragma once
#include"../utils/MVP_mat.h"
#include "../Screen.h"
#include "../shapes/Mesh.h"
#include "../utils/MeshRenderer.h"
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Shape
{

public:
	Shape(Shape&) = delete;
	Shape() = default;
	Shape(std::unique_ptr<MeshRenderer> mr) : mshRndr(std::move(mr)) {}

	void addChild(std::shared_ptr<Shape>& s)
	{
		childs.push_back(s);
	}
	
	void drawShape(Screen &screen,const MVP_mat& parent_trans)const //meant to be used only with world obj
	{
		MVP_mat thisTrans(parent_trans);
		thisTrans.model = parent_trans.model * glm::translate(glm::mat4(1), position) * rotation * glm::scale(glm::mat4(1), scaling);

		for (int i = 0; i < childs.size(); ++i)
		{
			childs[i]->drawChild(screen, thisTrans);
		}
	}

	void translate(const glm::vec3& v)
	{
		position = glm::translate(glm::mat4(1), v)*glm::vec4(position, 1);
	}
	void setPos(const glm::vec3& v)
	{
		position = v;
	}
	
	void rotate(float angle, glm::vec3 v)
	{
		rotation = glm::rotate(rotation, glm::radians(angle), v);
	}
	void scale(const glm::vec3& factor)
	{
		scaling *= factor;
	}
	void setScale(const glm::vec3& v)
	{
		scaling = v;
	}
	glm::vec3 getPos()const
	{
		return position;
	}
private:

	void drawChild(Screen &screen,const MVP_mat& parent_trans)const
	{
		MVP_mat thisTrans(parent_trans);
		thisTrans.model = parent_trans.model * glm::translate(glm::mat4(1),position) * rotation * glm::scale(glm::mat4(1),scaling);

		if (mshRndr.get() != nullptr)
			mshRndr->drawMesh(screen,thisTrans);

		for (int i = 0; i < childs.size(); ++i)
		{
			childs[i]->drawChild(screen,thisTrans);
		}
	}

	std::vector<std::shared_ptr<Shape>> childs;
	std::unique_ptr<MeshRenderer> mshRndr;
	glm::vec3 position = glm::vec3(0.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::vec3 scaling = glm::vec3(1.0f);


};

