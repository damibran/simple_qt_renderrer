#pragma once
#include "../MyMain/Screen.h"
#include "../utils/Mesh.h"
#include "../RendererComponent.h"
#include "../Shaders/Shader.h"

class ShaderMeshRenderer : public RendererComponent
{
public:
	~ShaderMeshRenderer() override = default;

	ShaderMeshRenderer(Screen& s, ShaderID shdr, const std::unique_ptr<Mesh>& m) : screen_(s),
		shader_(shdr), mesh_(m), pool_(s.pool_)
	{
	}

	void drawShapeVisual(const MVPMat& trans) override
	{
		drawMesh(screen_, trans, mesh_);
	}

protected:
	void drawMesh(Screen& screen, const MVPMat& trans, std::unique_ptr<Mesh> const& mesh)
	{
		for (size_t i = 0; !mesh->indices.empty() && i <= mesh->indices.size() - 3; i += 3)
		{
			pool_.push_task(std::move(std::make_unique<TaskArgs>(shader_, trans, mesh->vertices[mesh->indices[i]],
			                                                     mesh->vertices[mesh->indices[i + 1]],
			                                                     mesh->vertices[mesh->indices[i + 2]])));
		}

		for (auto const& i : mesh->childs)
		{
			drawMesh(screen, trans, i);
		}

		screen_.pool_.wait_for_tasks();
	}

	Screen& screen_;
	ShaderID shader_;
	const std::unique_ptr<Mesh>& mesh_;
	thread_pool& pool_;
};
