#pragma once
#include <memory>


class Entity;
// 每帧由 Window::Run 在调用 renderCallback 前更新
class RenderContext
{
public:
	float deltaTime = 0.f;
	int width = 0;
	int height = 0;
	std::shared_ptr<Entity> currentCamera;

};
