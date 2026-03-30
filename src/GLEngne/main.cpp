#include "../Engine/Renderer/RenderContext.hpp"
#include "../Engine/Renderer/AssetManager.hpp"





int main() 
{
	RenderContext context;
	context.Init(800,600,"GLEngine");
	AssetManager::Get()->LoadShader("assets/shaders/SampleShader.glsl");
	AssetManager::Get()->LoadModel("assets/models/Cone.fbx");


	context.Run();

	return 0;
}