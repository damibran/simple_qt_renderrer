#pragma once
#include <QSlider>
#include <memory>
#include "../Shaders/ConcreteShaders/OnePointSourceLitShader.h"
#include "../Shaders/ConcreteShaders/OnePointSourceLitShaderWithWireframe.h"
#include "../Renderers/ConcreteRenderers/ShaderMeshRenderer.h"
#include "../MyMain/Shape.h"
#include "../Script.h"

class BezierSurfaceScript final : public Script
{
public:
	BezierSurfaceScript(QSlider* xrot,QSlider* yrot,Transform* t): x_rot_slider_(xrot), y_rot_slider_(yrot),trans_(t)
	{}

	std::unique_ptr<Shape> static createObject(Ui::RenderrerMainWindowClass& ui, Screen& s,
	                                           Transform* light_source_transform, const int patch_u, const int patch_v,
	                                           const int res_u, const int res_v, const std::string& control_mesh_path)
	{
		std::unique_ptr<Transform> t(new Transform);
		t->translate({0, 0, 0});
		t->scale({10, 10, 10});
		auto shader = std::make_unique<OnePointSourceLitShaderWithWireframe>(light_source_transform);
		auto renderer = std::make_unique<ShaderMeshRenderer>(s, std::move(shader),
		                                                     generateBezierSurface(
			                                                     control_mesh_path, patch_u, patch_v, res_u, res_v));

		auto shp = std::make_unique<Shape>(std::move(t), std::move(renderer));

		shp->setScript(std::make_unique<BezierSurfaceScript>(ui.XRotSlider, ui.YRotSlider, shp->getTransformPtr()));

		return shp;
	}

	void updateScript(float dt) override
	{
		trans_->setRotationDegrees({ x_rot_slider_->value(), y_rot_slider_->value(), 0 });
	}

private:
	QSlider* x_rot_slider_;
	QSlider* y_rot_slider_;
	Transform* trans_;

	static glm::vec3 lerp(const glm::vec3& a, const glm::vec3 b, float t)
	{
		return (1 - t) * a + t * b;
	}

	static glm::vec3 evalBezierCurve(const std::vector<glm::vec3>& P, float t)
	{
		if (P.size() > 1)
		{
			std::vector<glm::vec3> next;
			next.reserve(P.size() - 1);
			for (int i = 0; i < P.size() - 1; ++i)
			{
				next.push_back(lerp(P[i], P[i + 1], t));
			}
			return evalBezierCurve(next, t);
		}
		else
			return P[0];
	}

	static glm::vec3 evalBezierPatch(const std::vector<glm::vec3>& control_points, int patch_u, const int patch_v,
	                                 const float& u, const float& v)
	{
		std::vector<glm::vec3> u_curve(patch_v);
		std::vector<glm::vec3> temp(patch_u);

		for (int i = 0; i < patch_v; ++i)
		{
			for (int j = 0; j < patch_u; ++j)
				temp[j] = control_points[i + patch_v * j];
			u_curve[i] = evalBezierCurve(temp, u);
		}
		return evalBezierCurve(u_curve, v);
	}

	static float angle(glm::vec3 a, glm::vec3 b)
	{
		return acos(dot(a, b) / (length(a) * length(b)));
	}

	static void calc_mesh_normals(std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& verts,
	                              const std::vector<unsigned int>& faces)
	{
		for (int i = 0; i < faces.size() / 3; ++i)
		{
			glm::vec3 v1 = verts[faces[i * 3 + 0]];
			glm::vec3 v2 = verts[faces[i * 3 + 1]];
			glm::vec3 v3 = verts[faces[i * 3 + 2]];

			glm::vec3 faceNorm = cross(v2 - v1, v3 - v1);

			float a1 = angle(v2 - v1, v3 - v1);
			float a2 = angle(v3 - v2, v1 - v2);
			float a3 = angle(v1 - v3, v2 - v3);

			normals[faces[i * 3 + 0]] += a1 * faceNorm;
			normals[faces[i * 3 + 1]] += a2 * faceNorm;
			normals[faces[i * 3 + 2]] += a3 * faceNorm;
		}

		for (int i = 0; i < verts.size(); ++i)
			normals[i] = normalize(normals[i]);
	}

	static std::unique_ptr<Mesh> generateBezierSurface(const std::string& control_mesh_path, const int patch_u,
	                                                   const int patch_v, const int res_u, const int res_v)
	// u corresponds to x, v to z in obj vertex data, in blender it is x and y
	// resolution is count of rows/columns per side count of vertices =+1 
	{
		const Mesh control_mesh = Mesh(control_mesh_path);

		std::vector<glm::vec3> controlVertices;
		controlVertices.reserve(control_mesh.getChildVerticesRef(0).size());

		for (auto& i : control_mesh.getChildVerticesRef(0))
			controlVertices.push_back(i.pos);

		std::sort(controlVertices.begin(), controlVertices.end(),
		          [](const glm::vec3& a, const glm::vec3& b)
		          {
			          return (a.x < b.x);
		          });
		for (int i = 0; i < patch_u; ++i)
		{
			std::sort(controlVertices.begin() + patch_v * i, controlVertices.begin() + patch_v * i + patch_v,
			          [](const glm::vec3& a, const glm::vec3& b)
			          {
				          return (a.z < b.z);
			          });
		}
		// after this indices data not valid

		std::vector<glm::vec3> unique_bezier_vertices;
		unique_bezier_vertices.reserve((res_u + 1) * (res_v + 1));
		std::vector<unsigned int> face_indices(res_u * res_v * 6);

		for (int i = 0, k = 0; i <= res_u; ++i)
		{
			float u = i / static_cast<float>(res_u);
			for (int j = 0; j <= res_v; ++j)
			{
				float v = j / static_cast<float>(res_v);
				unique_bezier_vertices.push_back(evalBezierPatch(controlVertices, patch_u, patch_v, u, v));
				if (i > 0 && j > 0)
				{
					glm::vec3 v1 = unique_bezier_vertices[i * (res_v + 1) + j] - unique_bezier_vertices[(i - 1) * (res_v
						+ 1) + j - 1];
					glm::vec3 v2 = unique_bezier_vertices[(i - 1) * (res_v + 1) + j] - unique_bezier_vertices[i * (res_v
						+ 1) + j - 1];

					if (glm::length(v1) >= glm::length(v2))
					{
						face_indices[k] = i * (res_v + 1) + j;
						face_indices[k + 1] = (i - 1) * (res_v + 1) + j - 1;
						face_indices[k + 2] = (i - 1) * (res_v + 1) + j;

						face_indices[k + 3] = i * (res_v + 1) + j;
						face_indices[k + 4] = i * (res_v + 1) + j - 1;
						face_indices[k + 5] = (i - 1) * (res_v + 1) + j - 1;
					}
					else
					{
						face_indices[k] = i * (res_v + 1) + j;
						face_indices[k + 1] = i * (res_v + 1) + j - 1;
						face_indices[k + 2] = (i - 1) * (res_v + 1) + j;

						face_indices[k + 3] = (i - 1) * (res_v + 1) + j - 1;
						face_indices[k + 4] = (i - 1) * (res_v + 1) + j;
						face_indices[k + 5] = i * (res_v + 1) + j - 1;
					}

					k += 6;
				}
			}
		}

		std::vector<glm::vec3> normals(unique_bezier_vertices.size());

		calc_mesh_normals(normals, unique_bezier_vertices, face_indices);

		std::vector<Vertex> vertices(unique_bezier_vertices.size());

		for (int i = 0; i < unique_bezier_vertices.size(); ++i)
		{
			vertices[i].pos = unique_bezier_vertices[i];
			vertices[i].norm = normals[i];
		}

		return std::make_unique<Mesh>(vertices, face_indices);
	}
};
