#pragma once
#include<QDebug>
#include"../utils/MVP_mat.h"
#include"../utils/Vertex.h"
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
	Mesh(std::vector<vertex> verts, std::vector<unsigned int> indes)
	{
		this->vertices = verts;
		this->indices = indes;
	}

private:
	// mesh Data
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::unique_ptr<Mesh>> childs;
	// model data 
	std::string directory;

	void loadMesh(std::string const& path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, 0);// aiProcess_Triangulate| aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
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

	std::unique_ptr<Mesh> processMesh(const aiMesh* mesh, const aiScene* scene)const
	{
		// data to fill
		std::vector<vertex> vertices;
		std::vector<unsigned int> indices;

		// walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vertex vrtx;

			vrtx.pos.x = mesh->mVertices[i].x;
			vrtx.pos.y = mesh->mVertices[i].y;
			vrtx.pos.z = mesh->mVertices[i].z;

			if (mesh->HasNormals())
			{
				vrtx.norm.x = mesh->mNormals[i].x;
				vrtx.norm.y = mesh->mNormals[i].y;
				vrtx.norm.z = mesh->mNormals[i].z;
			}

			vertices.push_back(vrtx);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// return a mesh object created from the extracted mesh data
		return std::make_unique<Mesh>(vertices, indices);
	}
};
