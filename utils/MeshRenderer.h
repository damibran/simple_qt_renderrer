#pragma once
#include "../shapes/Mesh.h"

class MeshRenderer
{
public:
	MeshRenderer(Shader& shdr, std::unique_ptr<Mesh> m) : shader(shdr), mesh(std::move(m)) {}
	void drawMesh(Screen& screen, const MVP_mat& trans)const
	{
		drawMesh(screen, trans, mesh);
	}
private:

	void drawMesh(Screen& screen, const MVP_mat& trans,  std::unique_ptr<Mesh> const &_mesh)const
	{
		for (int i = 0; _mesh->indices.size() != 0 && i <= _mesh->indices.size() - 3; i += 3)
		{
			screen.process_trngl(shader, trans, _mesh->vertices[_mesh->indices[i]], _mesh->vertices[_mesh->indices[i + 1]], _mesh->vertices[_mesh->indices[i + 2]]);
		}

		for (int i = 0; i < _mesh->childs.size(); ++i)
		{
			drawMesh(screen, trans, _mesh->childs[i]);
		}
	}
	Shader& shader;
	const std::unique_ptr<Mesh> mesh;
};