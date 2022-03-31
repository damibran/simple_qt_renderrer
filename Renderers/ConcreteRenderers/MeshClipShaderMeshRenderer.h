#pragma once
#include <unordered_set>
#include "../MyMain/Screen.h"
#include "../utils/Mesh.h"
#include "ShaderMeshRenderer.h"
#include "../Shaders/Shader.h"
#include "../utils/Transform.h"

struct Trngl
{
	Trngl(Vertex* a_, Vertex* b_, Vertex* c_): a(a_), b(b_), c(c_)
	{
	}

	Vertex* a;
	Vertex* b;
	Vertex* c;
};

class MeshClipShaderMeshRenderer : public ShaderMeshRenderer
{
public:
	MeshClipShaderMeshRenderer(Screen& s, std::unique_ptr<Shader> shdr, std::unique_ptr<Mesh> m,
	                           std::unique_ptr<Mesh> c_m, Transform* c_t) :
		ShaderMeshRenderer(s, std::move(shdr), std::move(m)), clip_trans(c_t), clip_mesh_(std::move(c_m->childs[0]))
	{
	}

	void drawShapeVisual(const MVPMat& trans) override
	{
		clipped_triangles_.clear();
		drawMesh(screen_, trans, mesh_);
	}

private:
	void drawMesh(Screen& screen, const MVPMat& trans, std::unique_ptr<Mesh> const& mesh)
	{
		for (size_t i = 0; i < mesh->indices.size() / 3; i++) // for every face(triangle) copy all vertices to set
		{
			clipped_triangles_.insert({
				&mesh->vertices[mesh->indices[i * 3]], &mesh->vertices[mesh->indices[i * 3 + 1]],
				&mesh->vertices[mesh->indices[i * 3 + 2]]
			});
		}

		for (size_t i = 0; i < clip_mesh_->indices.size() / 3; i++) // for every clip face(triangle) triangles in set
		{
			clipSetTriangles(clipped_triangles_, clip_mesh_->vertices[mesh->indices[i]],
			                 clip_mesh_->vertices[mesh->indices[i + 1]],
			                 clip_mesh_->vertices[mesh->indices[i + 2]]);
		}

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

	void clipSetTriangles(std::unordered_set<Trngl>& set, const Vertex& a, const Vertex& b, const Vertex& c)
	// abd is face triangle
	{
		for (const auto& trngl : set)
		{
			bool trngl_a_side = isInside(trngl.a, a, b, c);
			bool trngl_b_side = isInside(trngl.b, a, b, c);
			bool trngl_c_side = isInside(trngl.c, a, b, c);

			if (!trngl_a_side && !trngl_b_side && !trngl_c_side)
			{
				set.erase(trngl);
				continue;
			}
			else
			{
				
			}
		}
	}

	bool isInside(const Vertex* v, const Vertex& a, const Vertex& b, const Vertex& c)
	{
	}

	std::vector<Trngl> clipTriangle(std::vector<Trngl>& trngls, uint cur_clip_mesh_face)
	{
		for (auto& t : trngls)
		{
			glm::vec3& face_norm = clip_mesh_->vertices[clip_mesh_->indices[cur_clip_mesh_face * 3]].norm;
			// need to perform transformations
			glm::vec3& face_P = clip_mesh_->vertices[clip_mesh_->indices[cur_clip_mesh_face * 3]].pos;
		}
	}

	Transform* clip_trans;
	std::unique_ptr<Mesh> clip_mesh_;
	std::unordered_set<Trngl> clipped_triangles_;
};
