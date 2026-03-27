#pragma once
#include "Asset/Model.hpp"
#include "../../Engine/Core/LoaderManager.hpp"




int64_t get_mtime(const std::string& filePath)
{
	struct stat result;
	if (stat(filePath.c_str(), &result) == 0) {
		auto mod_time = result.st_mtime;
		return mod_time;
	}
	return -1;
}



struct FileState
{
	std::string path;
	std::shared_ptr<Shader> relativeAsset;
	int64_t mTime;
	bool IsNeedReload() 
	{
		if (get_mtime(path) != mTime)
			return true;
		return false;
	}
};


class AssetManager
{
public:
	~AssetManager()
	{
		delete instance;
		instance = nullptr;
	}
	static AssetManager* Get() 
	{
		if (!instance) {
			instance = new AssetManager();
		}
		return instance;
	}
	void ReloadAssets()
	{
		for (auto& file : shaderFiles) 
		{
			if (file.IsNeedReload()) 
			{
				int a = 4;
			}
		}

	}
	void LoadShader(const std::string&path) 
	{
		size_t lashSlashIndex = path.find_last_of("/");
		std::string shaderName = path.substr(lashSlashIndex + 1);
		std::vector<std::string> files;
		std::vector<std::string> passes = LoaderManager::loadshaderPasses(path, files);

	}
	void LoadTexture(const std::string& path) 
	{
		m_textures.push_back(LoaderManager::LoadTextureFromFile(path.c_str()));
	}
private:
	static AssetManager* instance;
	AssetManager() = default;
	std::vector<std::shared_ptr<Shader>> m_shaders;
	std::vector<std::shared_ptr<Texture>> m_textures;
	std::vector<FileState> shaderFiles;
};

AssetManager* AssetManager::instance = nullptr;