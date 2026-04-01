#pragma once
#include "../../Engine/Core/LoaderManager.hpp"
#include "Asset/Material.hpp"
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>



class AssetManager
{
public:
	~AssetManager() = default;
	//禁止拷贝
	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;
	static AssetManager& Get() 
	{
		static AssetManager instance;
		return instance;
	}
	std::shared_ptr<Shader> LoadShader(const std::string&path)
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>();
		LoaderManager::Get().LoadShader(path, shader);
		m_shaders[shader->m_name] = shader;
		return shader;
	}
	std::shared_ptr<Texture> LoadTexture(const std::string& path)
	{
		std::shared_ptr<Texture> tex = std::make_shared<Texture>();
		if (LoaderManager::Get().LoadTextureFromFile(path.c_str(), tex)) 
		{
			m_textures[tex->m_name] = tex;
		}
		return tex;
	}
	std::shared_ptr<Model> LoadModel(const std::string& path)
	{
		std::shared_ptr<Model> model = std::make_shared<Model>();
		LoaderManager::Get().LoadModel(path, model);
		m_models[model->m_name] = model;
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
		return nullptr;
	}

	template <typename T>
	std::shared_ptr<T> GetAsset(const std::string& name)
	{
		if constexpr (std::is_same_v<T, Shader>)
		{
			auto it = m_shaders.find(name);
			return it == m_shaders.end() ? std::shared_ptr<Shader>{} : it->second;
		}
		else if constexpr (std::is_same_v<T, Texture>)
		{
			auto it = m_textures.find(name);
			return it == m_textures.end() ? std::shared_ptr<Texture>{} : it->second;
		}
		else if constexpr (std::is_same_v<T, Model>)
		{
			auto it = m_models.find(name);
			return it == m_models.end() ? std::shared_ptr<Model>{} : it->second;
		}
		else if constexpr (std::is_same_v<T, Material>)
		{
			auto it = m_materials.find(name);
			return it == m_materials.end() ? std::shared_ptr<Material>{} : it->second;
		}
		else
		{
			static_assert(!sizeof(T), "AssetManager::GetAsset<T>: unsupported asset type T");
			return {};
		}
	}
	
private:
	AssetManager() 
	{
		//加载一些默认材质
		CreateMaterial("DefaultMaterial", LoadShader("assets/shaders/DefaultShader.glsl"));

	};
private:
	std::unordered_map<std::string,std::shared_ptr<Shader>> m_shaders;
	std::unordered_map<std::string,std::shared_ptr<Texture>> m_textures;
	std::unordered_map<std::string,std::shared_ptr<Model>> m_models;
	std::unordered_map<std::string,std::shared_ptr<Material>> m_materials;
};

