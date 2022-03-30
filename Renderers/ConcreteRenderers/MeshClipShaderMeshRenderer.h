#pragma once
#include "../MyMain/Screen.h"
#include "../utils/Mesh.h"
#include "ShaderMeshRenderer.h"
#include "../Shaders/Shader.h"
#include "../utils/Transform.h"

struct Trngl
{
	Vertex a;
	Vertex b;
	Vertex c;
};

class MeshClipShaderMeshRenderer : public ShaderMeshRenderer
{
public:
	MeshClipShaderMeshRenderer(Screen& s, std::unique_ptr<Shader> shdr, std::unique_ptr<Mesh> m,
	                           std::unique_ptr<Mesh> c_m, Transform* c_t) :
		ShaderMeshRenderer(s, std::move(shdr), std::move(m)), clip_trans(c_t)
	{
		clip_mesh_ = std::move(c_m->childs[0]);
	}

	void drawShapeVisual(const MVPMat& trans) override
	{
		drawMesh(screen_, trans, mesh_);
	}

private:

	void drawMesh(Screen& screen, const MVPMat& trans, std::unique_ptr<Mesh> const& mesh)
	{
		for (size_t i = 0; !mesh->indices.empty() && i <= mesh->indices.size() - 3; i += 3)
		{
			process_trngl(shader_, trans, mesh->vertices[mesh->indices[i]], mesh->vertices[mesh->indices[i + 1]],
			              mesh->vertices[mesh->indices[i + 2]]);
		}

		for (auto const& i : mesh->childs)
		{
			drawMesh(screen, trans, i);
		}
	}

	std::vector<Trngl> clipTriangle(std::vector<Trngl>& trngls, uint cur_clip_mesh_face)
	{
		for(auto& t: trngls)
		{
			glm::vec3& face_norm = clip_mesh_->vertices[clip_mesh_->indices[cur_clip_mesh_face*3]].norm; // need to perfom transformations
			glm::vec3& face_P = clip_mesh_->vertices[clip_mesh_->indices[cur_clip_mesh_face * 3]].pos;


		}
	}

	Transform* clip_trans;
	std::unique_ptr<Mesh> clip_mesh_;
};
