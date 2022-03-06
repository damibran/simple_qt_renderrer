#pragma once
#include "../utils/MVPMat.h"

class RendererComponent
{
public:
	virtual ~RendererComponent() = default;
	virtual void drawShapeVisual(const MVPMat& trans) = 0;
};