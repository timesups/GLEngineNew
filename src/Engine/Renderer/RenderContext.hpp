#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>//负责创建opengl的窗口
#include <memory>



#include "AssetManager.hpp"
#include "../Core/Window.hpp"
#include "../Core/LoaderManager.hpp"


void RenderLoop() 
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	LoaderManager::Get()->UpdateAssetFromDisk();
}


class RenderContext
{
public:
	RenderContext() = default;
	void Init(const int width,const int height,const char* title)
	{
		//初始化渲染器
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		win = std::make_unique<Window>();
		win->Create(width, height, title);

		win->renderCallback = RenderLoop;
	}
	void Run() 
	{
		win->Run();
	}
private:
	std::unique_ptr<Window> win;
};



