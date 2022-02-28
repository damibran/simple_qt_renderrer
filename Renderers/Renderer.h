#pragma once
#include "../utils/MVP_mat.h"


class Renderer
{
public:
	virtual void drawShapeVisual(const MVP_mat& trans) = 0;
};