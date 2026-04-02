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
	MeshRender(std::shared_ptr<Model> model):m_model(model)
	{
		
	};
	~MeshRender() = default;

	void SetModel(std::shared_ptr<Model> model) 
	{
		m_model = model;
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
			std::shared_ptr<Material> mat;
			if(section.materialIndex < m_materialOverride.size())
			{
				mat = m_materialOverride[section.materialIndex];
			}
			else if(section.materialIndex < m_model->m_materials.size())
			{
				mat = m_model->m_materials[section.materialIndex];
			}
			else
			{
				mat = AssetManager::Get().GetAsset<Material>("DefaultMaterial");
			}
			Transform* transform = GetEntity()->GetComponent<Transform>();
			if(transform)
				mat->Apply(section,transform->GetModelMatrix(),transform->GetNormalMatrix());
			else
				mat->Apply(section,glm::mat4(1),glm::mat4(1));

		}
	}
	virtual void Init() override
	{

	}
	virtual void Update(float deltaTime) override
	{

	}

private:
	std::shared_ptr<Model> m_model;
	std::vector<std::shared_ptr<Material>> m_materialOverride;
};

