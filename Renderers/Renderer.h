#pragma once
#include "../shapes/shape.h"


class Renderer
{
public:
	virtual void drawShapeVisual(const MVP_mat& trans) = 0;
};