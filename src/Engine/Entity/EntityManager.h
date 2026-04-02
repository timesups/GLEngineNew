#pragma once
#include "Entity.h"
#include "Components/Camera.h"
#include "Components/MeshRender.h"
#include "../Renderer/Asset/Model.h"
#include "../Renderer/AssetManager.h"

#include <memory>
#include <string>
#include <vector>

class EntityManager
{
public:
	static EntityManager& Get();
	~EntityManager() = default;
	EntityManager(const EntityManager&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;

	std::shared_ptr<Entity> CreateEntity(const std::string& name);
	std::shared_ptr<Entity> CreateCameraEntity(const std::string& name);
	std::shared_ptr<Entity> CreateMeshRenderEntity(const std::string& name, std::shared_ptr<Model> model);
	std::shared_ptr<Entity> CreateMeshRenderEntity(const std::string& name, const std::string& modelPath);

	void DestroyEntity(Entity* entity);
	void DestroyEntity(const std::shared_ptr<Entity>& entity);
	void Init();
	void Update(float deltaTime);
	void Render();
	void Clear();
	const std::vector<std::shared_ptr<Entity>>& GetEntities() const { return m_entities; }
	size_t GetEntityCount() const { return m_entities.size(); }

private:
	EntityManager() = default;
	std::vector<std::shared_ptr<Entity>> m_entities;
};
