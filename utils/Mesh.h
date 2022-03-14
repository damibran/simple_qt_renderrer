#pragma once
#include"../utils/Vertex.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh
{
public:
	// constructor, expects a filepath to a 3D model.
	Mesh(std::string const& path)
	{
		loadMesh(path);
	}

	friend class ShaderMeshRenderer;

	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indes)
	{
		this->vertices = verts;
		this->indices = indes;
	}

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
		std::vector<glm::vec3> uCurve(patch_v);

		for (int i = 0; i < patch_v; ++i)
			uCurve[i] = evalBezierCurve(
				std::vector(control_points.begin() + patch_u * i, control_points.begin() + patch_u * i + patch_u),
				u);
		return evalBezierCurve(uCurve, v);
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

	static std::unique_ptr<Mesh> generateBezierSurface(const std::string& control_mesh_path, int patch_u, int patch_v)
	// u corresponds to x, v to y
	{
		const Mesh control_mesh = Mesh(control_mesh_path);
		int resolution_u_v = 4; // count of columns/rows per side, count of vertices would be +1 

		std::vector<glm::vec3> controlVertices;
		controlVertices.reserve(control_mesh.childs[0]->vertices.size());

		for (auto& i : control_mesh.childs[0]->vertices)
			controlVertices.push_back(i.pos);

		std::sort(controlVertices.begin(), controlVertices.end(),
		          [](const glm::vec3& a, const glm::vec3& b)
		          {
			          return a.x < b.x || a.x == b.x && a.y < b.y || a.x == b.x && a.y == b.y && a.z < b.z;
		          });
		// after this indices data not valid

		std::vector<glm::vec3> unique_bezier_vertices;
		unique_bezier_vertices.reserve(resolution_u_v * resolution_u_v);
		std::vector<unsigned int> face_indices(resolution_u_v * resolution_u_v * 6);

		for (int i = 0, k = 0; i <= resolution_u_v; ++i)
		{
			float v = i / static_cast<float>(resolution_u_v);
			for (int j = 0; j <= resolution_u_v; ++j)
			{
				float u = j / static_cast<float>(resolution_u_v);
				unique_bezier_vertices.push_back(evalBezierPatch(controlVertices, patch_u, patch_v, u, v));
				if (i > 0 && j > 0)
				{
					face_indices[k] = i * (resolution_u_v + 1) + j;
					face_indices[k + 1] = i * (resolution_u_v + 1) + j - 1;
					face_indices[k + 2] = (i - 1) * (resolution_u_v + 1) + j - 1;

					face_indices[k + 3] = i * (resolution_u_v + 1) + j;
					face_indices[k + 4] = (i - 1) * (resolution_u_v + 1) + j - 1;
					face_indices[k + 5] = (i - 1) * (resolution_u_v + 1) + j;

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

private:
	// mesh Data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::unique_ptr<Mesh>> childs;
	// model data 
	std::string directory;

	void loadMesh(std::string const& path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices);
		// aiProcess_Triangulate| aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::string err = importer.GetErrorString();
			//gScreen.debug_massage("ERROR::ASSIMP:: "+ err);
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(const aiNode* node, const aiScene* scene)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->childs.push_back(std::move(processMesh(mesh, scene)));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	std::unique_ptr<Mesh> processMesh(const aiMesh* mesh, const aiScene* scene) const
	{
		// data to fill
		std::vector<Vertex> vertices_t;
		std::vector<unsigned int> indices_t;

		// walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vrtx;

			vrtx.pos.x = mesh->mVertices[i].x;
			vrtx.pos.y = mesh->mVertices[i].y;
			vrtx.pos.z = mesh->mVertices[i].z;

			if (mesh->HasNormals())
			{
				vrtx.norm.x = mesh->mNormals[i].x;
				vrtx.norm.y = mesh->mNormals[i].y;
				vrtx.norm.z = mesh->mNormals[i].z;
			}

			vertices_t.push_back(vrtx);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices_t.push_back(face.mIndices[j]);
		}

		// return a mesh object created from the extracted mesh data
		return std::make_unique<Mesh>(vertices_t, indices_t);
	}
};
