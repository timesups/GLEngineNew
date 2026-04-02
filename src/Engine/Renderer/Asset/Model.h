#pragma once

#include "Mesh.h"
#include "Material.h"


class Model 
{
public:
	Model() = default;
	void AddSection(std::shared_ptr<Mesh> mesh)
	{
		MeshSection section;
		section.mesh = mesh;
		section.materialIndex = m_meshSections.size();
		section.name = mesh->m_name;
		m_meshSections.push_back(section);
	}
	void addMaterial(std::shared_ptr<Material> mat) 
	{
		m_materials.push_back(mat);
	}
public:
	std::string m_name;
	std::string m_path;
	std::vector<MeshSection> m_meshSections;
	std::vector<std::shared_ptr<Material>> m_materials;
};