#pragma once
#include "glm/ext/matrix_float4x4.hpp"
#include <memory>

#include "../../Renderer/Asset/Model.h"
#include "Component.h"
#include "Transform.h"
#include "../Entity.h"
#include "../../Renderer/AssetManager.h"



class MeshRender :public Component 
{
public:
	MeshRender(std::shared_ptr<Model> model)
	{
		SetModel(model);
	};
	~MeshRender() = default;

	void SetModel(std::shared_ptr<Model> model) 
	{
		m_materialOverride.clear();
		m_model = model;
		m_materialOverride.resize(model->m_materials.size());
	}
	virtual void Init() override
	{

	}
	virtual void Update(float deltaTime) override
	{

	}
	//渲染
	virtual void Render() override
	{
		//如果没有模型，则不渲染
		if(!m_model)
			return;
		//遍历所有的section
		for(auto& section:m_model->m_meshSections)
		{
			std::shared_ptr<Material> mat = m_materialOverride[section.materialIndex];
			if(!mat)
				mat = m_model->m_materials[section.materialIndex];
			if(!mat)
				mat = AssetManager::Get().GetAsset<Material>("DefaultMaterial");
			Transform* transform = GetEntity()->GetComponent<Transform>();
			if(transform)
				mat->Apply(section,transform->GetModelMatrix(),transform->GetNormalMatrix());
			else
				mat->Apply(section,glm::mat4(1),glm::mat4(1));
		}
	}

	void SetMaterial(const int index,std::shared_ptr<Material> mat)
	{
		if(index > m_materialOverride.size())
			return;
		if(!mat)
			return;;
		m_materialOverride[index] = mat;
	}
private:
	std::shared_ptr<Model> m_model;
	std::vector<std::shared_ptr<Material>> m_materialOverride;
};

