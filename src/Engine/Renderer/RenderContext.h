#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "../Entity/EntityManager.h"
#include "../Entity/Components/Transform.h"
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
	/// 3D 场景宽高比与逻辑尺寸（与 sceneViewport* 一致，通常为 Dock 中央区）
	int width = 0;
	int height = 0;
	/// GLFW 帧缓冲整窗像素（停靠 UI 后也不变）
	int framebufferWidth = 0;
	int framebufferHeight = 0;
	/// 本帧 glViewport 用的场景区域（左下角原点、帧缓冲像素）
	int sceneViewportX = 0;
	int sceneViewportY = 0;
	int sceneViewportWidth = 0;
	int sceneViewportHeight = 0;
	std::shared_ptr<Entity> currentCamera;

};
