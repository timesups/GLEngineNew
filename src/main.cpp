#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/AssetManager.hpp"
#include "Engine/Renderer/Renderer.hpp"






int main() 
{
	//资源加载
	AssetManager::Get()->LoadShader("assets/shaders/DefaultShader.glsl");
	AssetManager::Get()->LoadModel("assets/models/Cone.fbx");

	//初始化渲染上下文
	RenderContext context;

	//初始化渲染器
	Renderer renderer;

	//初始化窗口
	std::unique_ptr<Window> win = std::make_unique<Window>();
	win->Create(800,600,"GLEngine");



	win->Run();

	return 0;
}