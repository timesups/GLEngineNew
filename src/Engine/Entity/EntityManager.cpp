#include "EntityManager.h"

#include <algorithm>

EntityManager& EntityManager::Get()
{
	static EntityManager instance;
	return instance;
}

std::shared_ptr<Entity> EntityManager::CreateEntity(const std::string& name)
{
	auto entity = std::make_shared<Entity>();
	entity->m_name = name;
	m_entities.push_back(entity);
	return entity;
}

std::shared_ptr<Entity> EntityManager::CreateCameraEntity(const std::string& name)
{
	auto entity = CreateEntity(name);
	entity->AddComponent<Transform>();
	entity->AddComponent<Camera>();
	return entity;
}

std::shared_ptr<Entity> EntityManager::CreateMeshRenderEntity(const std::string& name, std::shared_ptr<Model> model)
{
	auto entity = CreateEntity(name);
	entity->AddComponent<Transform>();
	entity->AddComponent<MeshRender>(std::move(model));
	return entity;
}

std::shared_ptr<Entity> EntityManager::CreateMeshRenderEntity(const std::string& name, const std::string& modelPath)
{
	return CreateMeshRenderEntity(name, AssetManager::Get().LoadModel(modelPath));
}

void EntityManager::DestroyEntity(Entity* entity)
{
	if (!entity)
		return;
	m_entities.erase(
		std::remove_if(m_entities.begin(), m_entities.end(),
			[entity](const std::shared_ptr<Entity>& p) { return p.get() == entity; }),
		m_entities.end());
}

void EntityManager::DestroyEntity(const std::shared_ptr<Entity>& entity)
{
	if (!entity)
		return;
	const auto it = std::find(m_entities.begin(), m_entities.end(), entity);
	if (it != m_entities.end())
		m_entities.erase(it);
}

void EntityManager::Init()
{
	for (auto& e : m_entities)
		e->Init();
}

void EntityManager::Update(float deltaTime)
{
	for (auto& e : m_entities)
		e->Update(deltaTime);
}

void EntityManager::Render()
{
	for (auto& e : m_entities)
		e->Render();
}

void EntityManager::Clear()
{
	m_entities.clear();
}
