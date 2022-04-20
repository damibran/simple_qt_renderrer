#pragma once
#include "../utils/MVPMat.h"

class RendererComponent
{
public:
	virtual ~RendererComponent() = default;
	virtual void drawShapeVisual(thread_pool& pool,const MVPMat& trans) = 0;
};