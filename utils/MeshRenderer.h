#pragma once
#include "../shapes/Mesh.h"
#include "../renderers/Rasterizer.h"

class MeshRenderer 
{
public:
	MeshRenderer(const Rasterizer& r, const Mesh& m) :raster(r), mesh(m) {}
	const Rasterizer& raster;
	const Mesh& mesh;
};