#pragma once
#include <unordered_set>
#define GLM_FORCE_EXPLICIT_CTOR
#include "../MyMain/Screen.h"
#include "../utils/Mesh.h"
#include "ShaderMeshRenderer.h"
#include "../Shaders/Shader.h"
#include "../utils/Transform.h"

struct Trngl
{
	Trngl(const Vertex& _a, const Vertex& _b, const Vertex& _c): a(_a), b(_b), c(_c)
	{
	}

	const Vertex& a;
	const Vertex& b;
	const Vertex& c;
};

class MeshClipShaderMeshRenderer : public ShaderMeshRenderer
{
public:
	~MeshClipShaderMeshRenderer() override = default;

	MeshClipShaderMeshRenderer(Screen& s, std::unique_ptr<Shader> shdr, std::unique_ptr<Mesh> m,
	                           std::unique_ptr<Mesh> c_m, Transform* c_t) :
		ShaderMeshRenderer(s, std::move(shdr), std::move(m)), clip_trans_(c_t), clip_mesh_(std::move(c_m->childs[0]))
	{
		clip_mesh_clip_space_vertices_.resize(clip_mesh_->vertices.size());
	}

	void drawShapeVisual(const MVPMat& trans) override
	{
		for (size_t i = 0; i < clip_mesh_->indices.size(); ++i)
		{
			clip_mesh_clip_space_vertices_[clip_mesh_->indices[i]].pos =
				trans.view *
				clip_trans_->getFullModelMatrix() *
				glm::vec4(clip_mesh_->vertices[clip_mesh_->indices[i]].pos, 1.0f);

			clip_mesh_clip_space_vertices_[clip_mesh_->indices[i]].norm =
				glm::normalize(
					glm::mat3(glm::transpose(glm::inverse(
						trans.view *
						clip_trans_->getFullModelMatrix()))) *
					clip_mesh_->vertices[clip_mesh_->indices[i]].norm);
		}
		drawMesh(screen_, trans, mesh_);
	}

private:
	int mainMeshIndx = 0;

	void drawMesh(Screen& screen, const MVPMat& trans, std::unique_ptr<Mesh> const& mesh)
	{
		mainMeshIndx = 0;
		for (size_t i = 0; !mesh->indices.empty() && i <= mesh->indices.size() - 3; i += 3)
		{
			TriangleClipPos abc = shader_->computeVertexShader(trans, mesh->vertices[mesh->indices[i]],
			                                                   mesh->vertices[mesh->indices[i + 1]],
			                                                   mesh->vertices[mesh->indices[i + 2]]);

			abc.a = glm::inverse(trans.proj) * abc.a;
			abc.b = glm::inverse(trans.proj) * abc.b;
			abc.c = glm::inverse(trans.proj) * abc.c;

			process_trngl(shader_, trans, abc, 0);
			mainMeshIndx++;
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

			float a_dist = distFromPlane2(abc.a, face_norm, face_P);
			float b_dist = distFromPlane2(abc.b, face_norm, face_P);
			float c_dist = distFromPlane2(abc.c, face_norm, face_P);

			//ordering CCW
			if (a_dist >= 0) // a is outside
			{
				if (b_dist >= 0) // b is outside too
				{
					if (c_dist >= 0) //c is outside
					{
						// a is outside b is outside c is outside
						if (mainMeshIndx == 0)
							qDebug() << "F + " << cur_clip_mesh_face << " : " << mainMeshIndx;
						return; // reject triangle
					}
					else
					{
						// a is outside b is outside c is inside
						float t_ca = c_dist / (c_dist - a_dist);
						float t_bc = b_dist / (b_dist - c_dist);
						glm::vec4 v_ca = abc.c + t_ca * (abc.a - abc.c);
						glm::vec4 v_bc = abc.b + t_bc * (abc.c - abc.b);
						std::unique_ptr<Shader> sh1 = shader->clone({t_ca, TriangleSide::CA}, {t_bc, TriangleSide::BC},
						                                            {0, TriangleSide::CA});
						process_trngl(sh1, trans, TriangleClipPos(v_ca, v_bc, abc.c), cur_clip_mesh_face + 1);
					}
				}
				else
				{
					if (c_dist > 0) //c is outside
					{
						// a is outside b is inside c is outside
						float t_ab = a_dist / (a_dist - b_dist);
						float t_bc = b_dist / (b_dist - c_dist);

						glm::vec4 v_ab = abc.a + t_ab * (abc.b - abc.a);
						glm::vec4 v_bc = abc.b + t_bc * (abc.c - abc.b);

						std::unique_ptr<Shader> sh1 = shader->clone({t_ab, TriangleSide::AB}, {0, TriangleSide::BC},
						                                            {t_bc, TriangleSide::BC});

						process_trngl(sh1, trans, {v_ab, abc.b, v_bc}, cur_clip_mesh_face + 1);
					}
					else // both c and b is inside
					{
						// a is outside b is inside c is inside
						float t_ab = a_dist / (a_dist - b_dist);
						float t_ca = c_dist / (c_dist - a_dist);

						glm::vec4 v_ab = abc.a + t_ab * (abc.b - abc.a);
						glm::vec4 v_ca = abc.c + t_ca * (abc.a - abc.c);

						std::unique_ptr<Shader> sh1 = shader->clone({t_ab, TriangleSide::AB}, {0, TriangleSide::BC},
						                                            {0, TriangleSide::CA});
						std::unique_ptr<Shader> sh2 = shader->clone({t_ab, TriangleSide::AB}, {0, TriangleSide::CA},
						                                            {t_ca, TriangleSide::CA});

						process_trngl(sh1, trans, {v_ab, abc.b, abc.c}, cur_clip_mesh_face + 1);
						process_trngl(sh2, trans, {v_ab, abc.c, v_ca}, cur_clip_mesh_face + 1);
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

						glm::vec4 v_ab = abc.a + t_ab * (abc.b - abc.a);
						glm::vec4 v_ca = abc.c + t_ca * (abc.a - abc.c);

						std::unique_ptr<Shader> sh1 = shader->clone({0, TriangleSide::AB}, {t_ab, TriangleSide::AB},
						                                            {t_ca, TriangleSide::CA});

						process_trngl(sh1, trans, {abc.a, v_ab, v_ca}, cur_clip_mesh_face + 1);
					}
					else
					{
						// a is inside b is outside c is inside
						float t_ab = a_dist / (a_dist - b_dist);
						float t_bc = b_dist / (b_dist - c_dist);

						glm::vec4 v_ab = abc.a + t_ab * (abc.b - abc.a);
						glm::vec4 v_bc = abc.b + t_bc * (abc.c - abc.b);

						std::unique_ptr<Shader> sh1 = shader->clone({0, TriangleSide::AB}, {t_ab, TriangleSide::AB},
						                                            {t_bc, TriangleSide::BC});
						std::unique_ptr<Shader> sh2 = shader->clone({0, TriangleSide::AB}, {t_bc, TriangleSide::BC},
						                                            {0, TriangleSide::CA});

						process_trngl(sh1, trans, {abc.a, v_ab, v_bc}, cur_clip_mesh_face + 1);
						process_trngl(sh2, trans, {abc.a, v_bc, abc.c}, cur_clip_mesh_face + 1);
					}
				}
				else
				{
					if (c_dist > 0) // c is outside
					{
						// a is inside b is inside c is outside
						float t_bc = b_dist / (b_dist - c_dist);
						float t_ca = c_dist / (c_dist - a_dist);

						glm::vec4 v_ca = abc.c + t_ca * (abc.a - abc.c);
						glm::vec4 v_bc = abc.b + t_bc * (abc.c - abc.b);

						std::unique_ptr<Shader> sh1 = shader->clone({0, TriangleSide::AB}, {0, TriangleSide::BC},
						                                            {t_ca, TriangleSide::CA});
						std::unique_ptr<Shader> sh2 = shader->clone({t_ca, TriangleSide::CA}, {0, TriangleSide::BC},
						                                            {t_bc, TriangleSide::BC});

						process_trngl(sh1, trans, {abc.a, abc.b, v_ca}, cur_clip_mesh_face + 1);
						process_trngl(sh2, trans, {v_ca, abc.b, v_bc}, cur_clip_mesh_face + 1);
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

	float distFromPlane(const glm::vec4& P, const glm::vec4& planeV)
	{
		return glm::dot(planeV, P);
	}

	float distFromPlane2(const glm::vec4& P, const glm::vec4& planeN, const glm::vec4& planeP)
	{
		return glm::dot(planeN, P - planeP);
	}

	Transform* clip_trans_;
	std::unique_ptr<Mesh> clip_mesh_;
	std::vector<VertexClip> clip_mesh_clip_space_vertices_;
};
