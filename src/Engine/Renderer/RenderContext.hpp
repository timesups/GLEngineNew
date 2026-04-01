#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "../Entity/EntityManager.hpp"
#include "../Entity/Components/Transform.hpp"
class Entity;
// 每帧由 Window::Run 在调用 renderCallback 前更新
class RenderContext
{
public:
	RenderContext()
	{
		Init();
	};
	void Init()
	{
		currentCamera = EntityManager::Get().CreateCameraEntity("MainCamera");
		currentCamera->GetComponent<Transform>()->SetPosition(glm::vec3(0,0,5));
	}
public:
	float deltaTime = 0.f;
	int width = 0;
	int height = 0;
	std::shared_ptr<Entity> currentCamera;

};
