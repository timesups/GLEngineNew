#include "EntityManager.h"

#include "../Core/Log.h"

#include <algorithm>

static constexpr const char* kModule = "EntityManager";

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
	Log(kModule, LogLevel::INFO, "CreateCameraEntity '{}'", name);
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
	Log(kModule, LogLevel::INFO, "CreateMeshRenderEntity '{}' model: {}", name, modelPath);
	return CreateMeshRenderEntity(name, AssetManager::Get().LoadModel(modelPath));
}

void EntityManager::DestroyEntity(Entity* entity)
{
	if (!entity)
	{
		Log(kModule, LogLevel::WARNING, "DestroyEntity called with null pointer");
		return;
	}
	Log(kModule, LogLevel::INFO, "DestroyEntity '{}'", entity->m_name);
	m_entities.erase(
		std::remove_if(m_entities.begin(), m_entities.end(),
			[entity](const std::shared_ptr<Entity>& p) { return p.get() == entity; }),
		m_entities.end());
}

void EntityManager::DestroyEntity(const std::shared_ptr<Entity>& entity)
{
	if (!entity)
	{
		Log(kModule, LogLevel::WARNING, "DestroyEntity(shared_ptr) called with empty ptr");
		return;
	}
	const auto it = std::find(m_entities.begin(), m_entities.end(), entity);
	if (it != m_entities.end())
	{
		Log(kModule, LogLevel::INFO, "DestroyEntity '{}'", entity->m_name);
		m_entities.erase(it);
	}
}

void EntityManager::Init()
{
	Log(kModule, LogLevel::INFO, "EntityManager::Init ({} entities)", m_entities.size());
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
	Log(kModule, LogLevel::INFO, "EntityManager::Clear");
	m_entities.clear();
}
