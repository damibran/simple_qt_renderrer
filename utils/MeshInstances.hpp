#pragma once
#include<unordered_map>
#include<memory>
#include"Mesh.h"

class MeshInstances
{
	std::unordered_map<std::string, std::unique_ptr<Mesh>> mesh_instances_;

public:

	void add(const std::string& s)
	{
		mesh_instances_[s] = std::make_unique<Mesh>(s);
	}

	const std::unique_ptr<Mesh>& get(const std::string& s) const
	{
		return mesh_instances_.at(s);
	}
};