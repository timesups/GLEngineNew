#include "Entity.h"

void Entity::Init()
{
	for (auto& [type, component] : m_components)
	{
		component->Init();
	}
}

void Entity::Update(float deltaTime)
{
	for (auto& [type, component] : m_components)
	{
		component->Update(deltaTime);
	}
}

void Entity::Render()
{
	for (auto& [type, component] : m_components)
	{
		component->Render();
	}
}
