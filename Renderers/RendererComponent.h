#pragma once
#include "../utils/MVP_mat.h"

class RendererComponent
{
public:
	virtual void drawShapeVisual(const MVP_mat& trans) = 0;
};