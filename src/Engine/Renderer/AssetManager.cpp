#include "AssetManager.h"

#include "Asset/Model.h"
#include "Asset/Shader.h"
#include "Asset/Texture.h"

AssetManager::AssetManager()
{
	//加载一些默认材质
	CreateMaterial("DefaultMaterial", LoadShader("assets/shaders/DefaultShader.glsl"));
}

std::shared_ptr<Shader> AssetManager::LoadShader(const std::string& path)
{
	std::shared_ptr<Shader> shader = std::make_shared<Shader>();
	LoaderManager::Get().LoadShader(path, shader);
	m_shaders[shader->m_name] = shader;
	return shader;
}

std::shared_ptr<Texture> AssetManager::LoadTexture(const std::string& path)
{
	std::shared_ptr<Texture> tex = std::make_shared<Texture>();
	if (LoaderManager::Get().LoadTextureFromFile(path.c_str(), tex))
	{
		m_textures[tex->m_name] = tex;
	}
	return tex;
}

std::shared_ptr<Model> AssetManager::LoadModel(const std::string& path)
{
	std::shared_ptr<Model> model = std::make_shared<Model>();
	LoaderManager::Get().LoadModel(path, model);
	m_models[model->m_name] = model;
	return model;
}

std::shared_ptr<Material> AssetManager::CreateMaterial(const std::string& name, std::shared_ptr<Shader> shader)
{
	std::shared_ptr<Material> mat = std::make_shared<Material>(shader);
	mat->m_name = name;
	m_materials[mat->m_name] = mat;
	return mat;
}

std::shared_ptr<Material> AssetManager::LoadMaterial(const std::string& path)
{
	//从文件加载Material的功能暂时不实现
	(void)path;
	return nullptr;
}

