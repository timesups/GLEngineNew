#pragma once
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
	std::shared_ptr<Shader> LoadShader(const std::string&path)
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>();
		LoaderManager::Get()->LoadShader(path, shader);
		m_shaders[shader->m_name] = shader;
		return shader;
	}
	std::shared_ptr<Texture> LoadTexture(const std::string& path)
	{
		std::shared_ptr<Texture> tex = std::make_shared<Texture>();
		if (LoaderManager::Get()->LoadTextureFromFile(path.c_str(), tex)) 
		{
			m_textures[tex->m_name] = tex;
		}
		return tex;
	}
	std::shared_ptr<Model> LoadModel(const std::string& path)
	{
		std::shared_ptr<Model> model = std::make_shared<Model>();
		LoaderManager::Get()->LoadModel(path, model);
		m_models[model->m_name] = model;

		//加载完成模型后向模型中添加默认材质
		for (auto& section : model->m_meshSections) 
		{
			model->addMaterial(m_defaultMaterial);
		}

		return model;
	}
	std::shared_ptr<Material> CreateMaterial(const std::string& name, std::shared_ptr<Shader> shader)
	{
		std::shared_ptr<Material> mat = std::make_shared<Material>(shader);
		mat->m_name = name;
		m_materials[mat->m_name] = mat;
		return mat;
	}
	std::shared_ptr<Material> LoadMaterial(const std::string&path)
	{
		//从文件加载Material的功能暂时不实现
	}
private:
	static AssetManager* instance;
	AssetManager() 
	{
		//加载一些默认材质
		m_defaultMaterial = CreateMaterial("DefaultMaterial", LoadShader("assets/shaders/DefaultShader.glsl"));

	};
private:
	std::unordered_map<std::string,std::shared_ptr<Shader>> m_shaders;
	std::unordered_map<std::string,std::shared_ptr<Texture>> m_textures;
	std::unordered_map<std::string,std::shared_ptr<Model>> m_models;
	std::unordered_map<std::string,std::shared_ptr<Material>> m_materials;

	std::shared_ptr<Material> m_defaultMaterial;
};

AssetManager* AssetManager::instance = nullptr;