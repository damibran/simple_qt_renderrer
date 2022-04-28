#pragma once
#include"../utils/Vertex.h"
#include "../utils/Texture.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "../utils/stb_image.h"
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

	Mesh(const std::unique_ptr<Mesh>& m): vertices(m->vertices), indices(m->indices) // dont copy childs
	{
		qDebug() << "Mesh Copy";
	}

	friend class ShaderMeshRenderer;
	friend class MeshClipShaderMeshRenderer;

	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& indes) : vertices(verts), indices(indes)
	{
	}

	const std::vector<Vertex>& getChildVerticesRef(int indx) const
	{
		return childs[indx]->vertices;
	}

	size_t faceCount() const
	{
		return indices.size() / 3;
	}

private:
	// mesh Data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Texture texture;
	std::vector<std::unique_ptr<Mesh>> childs;
	// model data 
	std::string directory;

	void loadMesh(std::string const& path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
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
			this->childs.push_back(processMesh(mesh, scene));
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

			if (mesh->mTextureCoords[0])
			{
				vrtx.texCoord.x = mesh->mTextureCoords[0][i].x;
				vrtx.texCoord.y = mesh->mTextureCoords[0][i].y;
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

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		Texture texture();

		// return a mesh object created from the extracted mesh data
		return std::make_unique<Mesh>(vertices_t, indices_t);
	}

	Texture loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		aiString str;
		mat->GetTexture(type, 0, &str);
		return TextureFromFile(str.C_Str(), this->directory);
	}

	Texture TextureFromFile(const char* path, const std::string& directory)
	{
		Texture texture;
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			texture.data=data;
			texture.width=width;
			texture.height=height;
			texture.nComp=nrComponents;
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return texture;
	}
};

