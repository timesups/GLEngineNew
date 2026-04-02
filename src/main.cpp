#include "Engine/Core/Log.h"
#include "Engine/Core/Window.h"
#include "Engine/Entity/Components/MeshRender.h"
#include "Engine/Entity/Components/Transform.h"
#include "Engine/Renderer/RenderContext.h"
#include "Engine/Renderer/AssetManager.h"
#include "Engine/Entity/EntityManager.h"
#include "Engine/Renderer/Renderer.h"

static constexpr const char* kModule = "Main";

int main() 
{
	Log(kModule, LogLevel::INFO, "GLEngine starting");
	//初始化窗口
	std::unique_ptr<Window> win = std::make_unique<Window>();
	if (!win->Create(800, 600, "GLEngine")) 
	{
		Log(kModule, LogLevel::ERROR, "Failed to create window!");
		return 0;
	}
	//初始化渲染上下文
	RenderContext context;
	//初始化渲染器
	Renderer renderer;
	Log(kModule, LogLevel::INFO, "Renderer initialized");
	win->renderCallback = [&renderer](RenderContext& ctx) {
		renderer.Render(ctx);
		};
	//资源加载
	AssetManager::Get().LoadShader("assets/shaders/DefaultShader.glsl");
	//准备场景
	auto model = EntityManager::Get().CreateMeshRenderEntity("Model", "assets/models/Cube.fbx");

	if (model)
		model->GetComponent<Transform>()->SetRotation(glm::vec3(0.0, 45.0, 0.0));
	auto material = AssetManager::Get().CreateMaterial("Material", AssetManager::Get().LoadShader("assets/shaders/DefaultTexture.glsl"));
	auto tex = AssetManager::Get().LoadTexture("assets/textures/container2.png");
	material->SetTexture2DProperty("tex", tex);
	model->GetComponent<MeshRender>()->SetMaterial(0, material);
	
	
	//初始化所有Entity
	EntityManager::Get().Init();
	Log(kModule, LogLevel::INFO, "Scene init done, {} entities", EntityManager::Get().GetEntityCount());

	win->Run(context);

	Log(kModule, LogLevel::INFO, "GLEngine shutdown");
	return 0;
}