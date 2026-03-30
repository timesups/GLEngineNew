#pragma once



#include "Mesh.hpp"
#include "Material.hpp"


struct MeshSection {
	std::shared_ptr<Mesh> mesh;
	int materialIndex;  // 指向材质数组的索引
	std::string name;
};

class Model 
{
public:
	Model() = default;
	void AddSection(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat,const std::string name)
	{
		MeshSection section;
		section.mesh = mesh;
		section.materialIndex = m_materials.size();
		m_materials.push_back(mat);
	}
public:
	std::string m_name;
	std::string m_path;
private:
	std::vector<MeshSection> m_meshSections;
	std::vector<std::shared_ptr<Material>> m_materials;
};