#include "AssetManager.h"

#include "../Core/Log.h"
#include "Asset/Model.h"
#include "Asset/Shader.h"
#include "Asset/Texture.h"

static constexpr const char* kModule = "AssetManager";

AssetManager::AssetManager()
{
	//加载一些默认材质
	CreateMaterial("DefaultMaterial", LoadShader("assets/shaders/DefaultShader.glsl"));
	Log(kModule, LogLevel::INFO, "Default assets initialized (DefaultMaterial + shader)");
}

std::shared_ptr<Shader> AssetManager::LoadShader(const std::string& path)
{
	std::shared_ptr<Shader> shader = std::make_shared<Shader>();
	if (!LoaderManager::Get().LoadShader(path, shader))
	{
		Log(kModule, LogLevel::ERROR, "LoadShader failed: {}", path);
		m_shaders[shader->m_name] = shader;
		return shader;
	}
	const bool firstRegister = (m_shaders.find(shader->m_name) == m_shaders.end());
	m_shaders[shader->m_name] = shader;
	if (firstRegister)
		Log(kModule, LogLevel::INFO, "Shader registered: '{}' ({})", shader->m_name, path);
	return shader;
}

std::shared_ptr<Texture> AssetManager::LoadTexture(const std::string& path)
{
	std::shared_ptr<Texture> tex = std::make_shared<Texture>();
	if (LoaderManager::Get().LoadTextureFromFile(path.c_str(), tex))
		m_textures[tex->m_name] = tex;
	else
		Log(kModule, LogLevel::ERROR, "LoadTexture failed: {}", path);
	return tex;
}

std::shared_ptr<Model> AssetManager::LoadModel(const std::string& path)
{
	std::shared_ptr<Model> model = std::make_shared<Model>();
	if (!LoaderManager::Get().LoadModel(path, model))
		Log(kModule, LogLevel::ERROR, "LoadModel failed: {}", path);
	m_models[model->m_name] = model;
	model->m_materials.resize(model->m_meshSections.size());
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
	(void)path;
	Log(kModule, LogLevel::WARNING, "LoadMaterial from file is not implemented ({})", path);
	return nullptr;
}

