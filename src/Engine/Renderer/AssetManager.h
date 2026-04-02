#pragma once
#include "../Core/LoaderManager.h"
#include "Asset/Material.h"
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
	std::shared_ptr<Shader> LoadShader(const std::string& path);
	std::shared_ptr<Texture> LoadTexture(const std::string& path);
	std::shared_ptr<Model> LoadModel(const std::string& path);
	std::shared_ptr<Material> CreateMaterial(const std::string& name, std::shared_ptr<Shader> shader);
	std::shared_ptr<Material> LoadMaterial(const std::string& path);

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
	AssetManager();
private:
	std::unordered_map<std::string,std::shared_ptr<Shader>> m_shaders;
	std::unordered_map<std::string,std::shared_ptr<Texture>> m_textures;
	std::unordered_map<std::string,std::shared_ptr<Model>> m_models;
	std::unordered_map<std::string,std::shared_ptr<Material>> m_materials;
};

