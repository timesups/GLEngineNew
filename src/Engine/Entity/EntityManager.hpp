#pragma once
#include "Entity.hpp"
#include "Components/Camera.hpp"
#include "Components/MeshRender.hpp"
#include "../Renderer/AssetManager.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

class EntityManager
{
public:
	~EntityManager() {}
	static EntityManager* Get() 
	{
		if (!instance)
			instance = new EntityManager();
		return instance;
	}
	std::shared_ptr<Entity> CreateEntity(const std::string& name)
	{
		auto entity = std::make_shared<Entity>();
		entity->m_name = name;
		m_entities.push_back(entity);
		return  entity;
	}

	std::shared_ptr<Entity> CreateCameraEntity(const std::string& name)
	{
		auto entity = CreateEntity(name);
		entity->AddComponent<Transform>();
		entity->AddComponent<Camera>();
		return entity;
	}

	std::shared_ptr<Entity> CreateMeshRenderEntity(const std::string& name, std::shared_ptr<Model> model)
	{
		auto entity = CreateEntity(name);
		entity->AddComponent<Transform>();
		entity->AddComponent<MeshRender>(std::move(model));
		return entity;
	}

	std::shared_ptr<Entity> CreateMeshRenderEntity(const std::string& name, const std::string& modelPath)
	{
		return CreateMeshRenderEntity(name, AssetManager::Get()->LoadModel(modelPath));
	}

	void DestroyEntity(Entity* entity)
	{
		if (!entity)
			return;
		m_entities.erase(
			std::remove_if(m_entities.begin(), m_entities.end(),
				[entity](const std::shared_ptr<Entity>& p) { return p.get() == entity; }),
			m_entities.end());
		
	}
	void DestroyEntity(const std::shared_ptr<Entity>& entity)
	{
		if (!entity)
			return;
		const auto it = std::find(m_entities.begin(), m_entities.end(), entity);
		if (it != m_entities.end())
			m_entities.erase(it);
	}
	void Init()
	{
		for (auto& e : m_entities)
			e->Init();
	}
	void Update(float deltaTime)
	{
		for (auto& e : m_entities)
			e->Update(deltaTime);
	}
	void Render()
	{
		for (auto& e : m_entities)
			e->Render();
	}
	void Clear()
	{
		m_entities.clear();
	}
	const std::vector<std::shared_ptr<Entity>>& GetEntities() const { return m_entities; }
	size_t GetEntityCount() const { return m_entities.size(); }
private:
	static EntityManager* instance;
	EntityManager() = default;
	std::vector<std::shared_ptr<Entity>> m_entities;
};
EntityManager* EntityManager::instance = nullptr;
