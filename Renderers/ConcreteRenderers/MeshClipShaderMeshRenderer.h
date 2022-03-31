#pragma once
#include <unordered_set>
#include "../MyMain/Screen.h"
#include "../utils/Mesh.h"
#include "ShaderMeshRenderer.h"
#include "../Shaders/Shader.h"
#include "../utils/Transform.h"

struct Trngl
{
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


	void process_trngl(std::unique_ptr<Shader>& shader, const MVPMat& trans, const Vertex& v0, const Vertex& v1,
	                   const Vertex& v2)
	{
		TriangleClipPos abc = shader->computeVertexShader(trans, v0, v1, v2);

		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 c;

		//calculating raster positions
		a.x = static_cast<float>(screen_.XMAX) * ((abc.v1.x / abc.v1.w + 1) / 2);
		a.y = static_cast<float>(screen_.YMAX) * ((abc.v1.y / abc.v1.w + 1) / 2);
		a.z = abc.v1.w;

		b.x = static_cast<float>(screen_.XMAX) * ((abc.v2.x / abc.v2.w + 1) / 2);
		b.y = static_cast<float>(screen_.YMAX) * ((abc.v2.y / abc.v2.w + 1) / 2);
		b.z = abc.v2.w;

		c.x = static_cast<float>(screen_.XMAX) * ((abc.v3.x / abc.v3.w + 1) / 2);
		c.y = static_cast<float>(screen_.YMAX) * ((abc.v3.y / abc.v3.w + 1) / 2);
		c.z = abc.v3.w;

		if (abc.v1.z <= abc.v1.w && abc.v1.z >= -abc.v1.w &&
			abc.v2.z <= abc.v2.w && abc.v2.z >= -abc.v2.w &&
			abc.v3.z <= abc.v3.w && abc.v3.z >= -abc.v3.w) //kinda frutsum Clipping
		{
			put_triangle(shader, a, b, c);
		}
	}

	std::vector<TriangleClipPos> clipTriangleByMesh(std::unordered_set<TriangleClipPos> ,Vertex& a, Vertex& b, Vertex& c)
	{
		for (size_t i = 0; !clip_mesh_->indices.empty() && i <= clip_mesh_->indices.size() - 3; i += 3)
		{
			glm::vec3& face_norm = clip_mesh_->vertices[clip_mesh_->indices[i]].norm;
			// need to perfom transformations
			glm::vec3& face_P = clip_mesh_->vertices[clip_mesh_->indices[i]].pos;
		}
	}

	bool isInside(Vertex& v)
	{

	}

	Transform* clip_trans;
	std::unique_ptr<Mesh> clip_mesh_;
};
