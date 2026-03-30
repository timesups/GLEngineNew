#pragma once
#include "Asset/Model.hpp"
#include "../../Engine/Core/LoaderManager.hpp"



class AssetManager
{
public:
	~AssetManager()
	{
		if (instance) 
		{
			delete instance;
			instance = nullptr;
		}
	}
	static AssetManager* Get() 
	{
		if (!instance) {
			instance = new AssetManager();
		}
		return instance;
	}
	void LoadShader(const std::string&path) 
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>();
		LoaderManager::Get()->LoadShader(path, shader);
		m_shaders[shader->m_name] = shader;
	}
	void LoadTexture(const std::string& path) 
	{
		std::shared_ptr<Texture> tex = std::make_shared<Texture>();
		if (LoaderManager::Get()->LoadTextureFromFile(path.c_str(), tex)) 
		{
			m_textures[tex->m_name] = tex;
		}
	}
	void LoadModel(const std::string& path) 
	{
		std::shared_ptr<Model> model = std::make_shared<Model>();
		LoaderManager::Get()->LoadModel(path, model);
		m_models[model->m_name] = model;
	}
private:
	static AssetManager* instance;
	AssetManager() 
	{
		//加载一些内置着色器


	};
private:
	std::unordered_map<std::string,std::shared_ptr<Shader>> m_shaders;
	std::unordered_map<std::string,std::shared_ptr<Texture>> m_textures;
	std::unordered_map<std::string,std::shared_ptr<Model>> m_models;
};

AssetManager* AssetManager::instance = nullptr;