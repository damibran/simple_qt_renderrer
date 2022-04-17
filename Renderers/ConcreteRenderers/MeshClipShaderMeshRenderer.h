#pragma once
#include <unordered_set>
#define GLM_FORCE_EXPLICIT_CTOR
#include "../MyMain/Screen.h"
#include "../utils/Mesh.h"
#include "ShaderMeshRenderer.h"
#include "../Shaders/ConcreteShaders/WireframeShader.h"
#include "../Shaders/Shader.h"
#include "../utils/Transform.h"

/*class MeshClipShaderMeshRenderer : public ShaderMeshRenderer
{
public:
	~MeshClipShaderMeshRenderer() override = default;

	MeshClipShaderMeshRenderer(Screen& s, std::unique_ptr<Shader> main_obj_shdr, const std::unique_ptr<Mesh>& m,
	                           const std::unique_ptr<Mesh>& c_m, std::unique_ptr<Transform>& c_t, bool r_o) :
		ShaderMeshRenderer(s, std::move(main_obj_shdr), m), clip_trans_(c_t), clip_mesh_(c_m->childs[0]),
		render_outer_(r_o)
	{
		clip_mesh_clip_space_vertices_.resize(clip_mesh_->vertices.size());
	}

	void drawShapeVisual(const MVPMat& trans) override
	{
		glm::mat3 t_inv_proj = glm::mat3(glm::transpose(glm::inverse(
			trans.view *
			clip_trans_->getFullModelMatrix())));
		for (size_t i = 0; i < clip_mesh_->indices.size(); ++i)
		{
			clip_mesh_clip_space_vertices_[clip_mesh_->indices[i]].pos =
				trans.view *
				clip_trans_->getFullModelMatrix() *
				glm::vec4(clip_mesh_->vertices[clip_mesh_->indices[i]].pos, 1.0f);

			clip_mesh_clip_space_vertices_[clip_mesh_->indices[i]].norm =
				glm::normalize(t_inv_proj *
					clip_mesh_->vertices[clip_mesh_->indices[i]].norm);
		}
		drawMesh(screen_, trans, mesh_);
	}

private:
	void drawMesh(Screen& screen, const MVPMat& trans, std::unique_ptr<Mesh> const& mesh)
	{
		glm::mat4 inv_proj = glm::inverse(trans.proj);
		glm::mat3 t_inv_MV = glm::mat3(glm::transpose(glm::inverse(trans.view * trans.model)));
		for (size_t i = 0; !mesh->indices.empty() && i <= mesh->indices.size() - 3; i += 3)
		{
			TriangleClipPos abc = shader_->computeVertexShader(trans,t_inv_MV, mesh->vertices[mesh->indices[i]],
			                                                   mesh->vertices[mesh->indices[i + 1]],
			                                                   mesh->vertices[mesh->indices[i + 2]]);

			abc.a = inv_proj * abc.a;
			abc.b = inv_proj * abc.b;
			abc.c = inv_proj * abc.c;

			process_trngl(shader_, trans, abc, 0);
		}

		for (auto const& i : mesh->childs)
		{
			drawMesh(screen, trans, i);
		}
	}

	void process_trngl(std::unique_ptr<Shader>& shader, const MVPMat& trans, const TriangleClipPos& abc,
	                   size_t cur_clip_mesh_face)
	{
		if (cur_clip_mesh_face == clip_mesh_->faceCount())
		{
			TriangleClipPos abc_(trans.proj * abc.a, trans.proj * abc.b, trans.proj * abc.c);

			glm::vec3 a;
			glm::vec3 b;
			glm::vec3 c;

			//calculating raster positions
			a.x = static_cast<float>(screen_.XMAX) * ((abc_.a.x / abc_.a.w + 1) / 2);
			a.y = static_cast<float>(screen_.YMAX) * ((abc_.a.y / abc_.a.w + 1) / 2);
			a.z = abc_.a.w;

			b.x = static_cast<float>(screen_.XMAX) * ((abc_.b.x / abc_.b.w + 1) / 2);
			b.y = static_cast<float>(screen_.YMAX) * ((abc_.b.y / abc_.b.w + 1) / 2);
			b.z = abc_.b.w;

			c.x = static_cast<float>(screen_.XMAX) * ((abc_.c.x / abc_.c.w + 1) / 2);
			c.y = static_cast<float>(screen_.YMAX) * ((abc_.c.y / abc_.c.w + 1) / 2);
			c.z = abc_.c.w;

			if (abc_.a.z <= abc_.a.w && abc_.a.z >= -abc_.a.w &&
				abc_.b.z <= abc_.b.w && abc_.b.z >= -abc_.b.w &&
				abc_.c.z <= abc_.c.w && abc_.c.z >= -abc_.c.w) //kinda frutsum Clipping
			{
				put_triangle(shader, a, b, c);
			}
		}
		else
		{
			glm::vec4 face_norm = glm::vec4(
				clip_mesh_clip_space_vertices_[clip_mesh_->indices[cur_clip_mesh_face * 3 + 1]].
				norm, 0);

			glm::vec4& face_P = clip_mesh_clip_space_vertices_[clip_mesh_->indices[cur_clip_mesh_face * 3 + 1]].
				pos;

			float a_dist = distFromPlane(abc.a, face_norm, face_P);
			float b_dist = distFromPlane(abc.b, face_norm, face_P);
			float c_dist = distFromPlane(abc.c, face_norm, face_P);

			//ordering CCW
			if (a_dist >= 0) // a is outside
			{
				if (b_dist >= 0) // b is outside too
				{
					if (c_dist >= 0) //c is outside
					{
						// a is outside b is outside c is outside
						if (render_outer_)
						{
							std::unique_ptr<Shader> sh1 = shader->clone({0, TriangleSide::AB}, {0, TriangleSide::BC},
							                                            {0, TriangleSide::CA});
							sh1->changeColor({255, 0, 0});
							process_trngl(sh1, trans, abc, clip_mesh_->faceCount());
						}
						else
							return; // reject triangle
					}
					else
					{
						// a is outside b is outside c is inside
						float t_ca = c_dist / (c_dist - a_dist);
						float t_bc = b_dist / (b_dist - c_dist);

						oneInsideTwoOutside(t_ca, TriangleSide::CA, t_bc, TriangleSide::BC, shader, trans, abc,
						                    cur_clip_mesh_face);
					}
				}
				else
				{
					if (c_dist > 0) //c is outside
					{
						// a is outside b is inside c is outside
						float t_ab = a_dist / (a_dist - b_dist);
						float t_bc = b_dist / (b_dist - c_dist);

						oneInsideTwoOutside(t_bc, TriangleSide::BC, t_ab, TriangleSide::AB, shader, trans, abc,
						                    cur_clip_mesh_face);
					}
					else // both c and b is inside
					{
						// a is outside b is inside c is inside
						float t_ab = a_dist / (a_dist - b_dist);
						float t_ca = c_dist / (c_dist - a_dist);

						twoInsideOneOutSide(t_ab, TriangleSide::AB, t_ca, TriangleSide::CA, shader, trans, abc,
						                    cur_clip_mesh_face);
					}
				}
			}
			else if (a_dist < 0) //a is inside
			{
				if (b_dist > 0) // b is outside
				{
					if (c_dist > 0) // c is outside
					{
						// a is inside b is outside c is outside
						float t_ab = a_dist / (a_dist - b_dist);
						float t_ca = c_dist / (c_dist - a_dist);

						oneInsideTwoOutside(t_ab, TriangleSide::AB, t_ca, TriangleSide::CA, shader, trans, abc,
						                    cur_clip_mesh_face);
					}
					else
					{
						// a is inside b is outside c is inside
						float t_ab = a_dist / (a_dist - b_dist);
						float t_bc = b_dist / (b_dist - c_dist);

						twoInsideOneOutSide(t_bc, TriangleSide::BC, t_ab, TriangleSide::AB, shader, trans, abc,
						                    cur_clip_mesh_face);
					}
				}
				else
				{
					if (c_dist > 0) // c is outside
					{
						// a is inside b is inside c is outside
						float t_bc = b_dist / (b_dist - c_dist);
						float t_ca = c_dist / (c_dist - a_dist);

						twoInsideOneOutSide(t_ca, TriangleSide::CA, t_bc, TriangleSide::BC, shader, trans, abc,
						                    cur_clip_mesh_face);
					}
					else
					{
						// a is inside b is inside c is inside
						process_trngl(shader, trans, abc, cur_clip_mesh_face + 1);
					}
				}
			}
		}
	}

	float distFromPlane(const glm::vec4& P, const glm::vec4& planeN, const glm::vec4& planeP) const
	{
		return glm::dot(planeN, P - planeP);
	}

	// {0 , side1 } - gives inside one and {1,side2} too
	void oneInsideTwoOutside(float t_side1, TriangleSide side1, float t_side2,
	                         TriangleSide side2, std::unique_ptr<Shader>& shader, const MVPMat& trans,
	                         const TriangleClipPos& abc,
	                         size_t cur_clip_mesh_face)
	{
		const glm::vec4 v_side1 = abc.lerpSide(t_side1, side1);
		const glm::vec4 v_side2 = abc.lerpSide(t_side2, side2);

		std::unique_ptr<Shader> sh1 = shader->clone({t_side1, side1}, {t_side2, side2},
		                                            {0, side1});

		process_trngl(sh1, trans, TriangleClipPos(v_side1, v_side2, abc.lerpSide(0, side1)), cur_clip_mesh_face + 1);

		if (render_outer_)
		{
			std::unique_ptr<Shader> sh2 = shader->clone({t_side1, side1}, {1, side1},
			                                            {0, side2});

			std::unique_ptr<Shader> sh3 = shader->clone({t_side1, side1}, {0, side2},
			                                            {t_side2, side2});

			sh2->changeColor({255, 0, 0});
			sh3->changeColor({255, 0, 0});

			process_trngl(sh2, trans, TriangleClipPos(v_side1, abc.lerpSide(1, side1), abc.lerpSide(0, side2)),
			              clip_mesh_->faceCount());
			process_trngl(sh3, trans, TriangleClipPos(v_side1, abc.lerpSide(0, side2), v_side2),
			              clip_mesh_->faceCount());
		}
	}

	// {0,side1} gives outside one and {1, side2} too
	void twoInsideOneOutSide(float t_side1, TriangleSide side1, float t_side2,
	                         TriangleSide side2, std::unique_ptr<Shader>& shader, const MVPMat& trans,
	                         const TriangleClipPos& abc,
	                         size_t cur_clip_mesh_face)
	{
		// a is outside b is inside c is inside
		glm::vec4 v_side1 = abc.lerpSide(t_side1, side1);
		glm::vec4 v_side2 = abc.lerpSide(t_side2, side2);

		std::unique_ptr<Shader> sh1 = shader->clone({t_side1, side1}, {1, side1},
		                                            {0, side2});
		std::unique_ptr<Shader> sh2 = shader->clone({t_side1, side1}, {0, side2},
		                                            {t_side2, side2});

		process_trngl(sh1, trans, {v_side1, abc.lerpSide(1, side1), abc.lerpSide(0, side2)}, cur_clip_mesh_face + 1);
		process_trngl(sh2, trans, {v_side1, abc.lerpSide(0, side2), v_side2}, cur_clip_mesh_face + 1);

		if (render_outer_)
		{
			std::unique_ptr<Shader> sh3 = shader->clone({0, side1}, {t_side1, side1},
			                                            {t_side2, side2});

			sh3->changeColor({255, 0, 0});

			process_trngl(sh3, trans, {abc.lerpSide(0, side1), v_side1, v_side2}, clip_mesh_->faceCount());
		}
	}

	std::unique_ptr<Transform>& clip_trans_;
	const std::unique_ptr<Mesh>& clip_mesh_;
	std::vector<VertexView> clip_mesh_clip_space_vertices_;
	const bool render_outer_ = false;
};
*/