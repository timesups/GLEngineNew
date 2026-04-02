#pragma once
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <sys/stat.h>

class Texture;
class Shader;
class Model;
struct aiNode;
struct aiScene;
struct aiMesh;
//utility
inline int64_t get_mtime(const std::string& filePath)
{
	struct stat result;
	if (stat(filePath.c_str(), &result) == 0) {
		auto mod_time = result.st_mtime;
		return mod_time;
	}
	return -1;
}

inline void stringToLower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](char& c) {
		return std::tolower(c); });
}

inline std::string getBlockContent(const std::string& content, const int startPoint)
{
	std::string blockContent;
	bool blockStartFlag = false;
	int bracketCount = 0;
	for (char c : content.substr(startPoint))
	{
		if (c == '{' and not blockStartFlag)
		{
			blockStartFlag = true;
			bracketCount += 1;
			continue;
		}
		else if (c == '{')
			bracketCount += 1;
		else if (c == '}')
			bracketCount -= 1;
		if (bracketCount == 0 and blockStartFlag)
		{
			break;
		}
		if (blockStartFlag)
		{
			blockContent += c;
		}
	}
	return blockContent;
}


template<typename T>
struct FileState
{
	FileState() = default;
	FileState(const std::string&path) 
	{
		this->path = path;
		UpdateModifyTime();
	}
	void AddRelativeAsset(std::shared_ptr<T> asset)
	{
		if (!asset)
			return;
		for (const auto& existing : relativeAsset)
		{
			if (existing == asset)
				return;
		}
		relativeAsset.push_back(std::move(asset));
	}
	void UpdateModifyTime() 
	{
		mTime = get_mtime(path);
	}
	std::string path;
	std::vector<std::shared_ptr<T>> relativeAsset;
	int64_t mTime;
	bool IsNeedReload()
	{
		if (get_mtime(path) != mTime)
			return true;
		return false;
	}
};

class LoaderManager
{
public:
	static LoaderManager& Get()
	{
		static LoaderManager instance;
		return instance;
	}
	~LoaderManager() = default;

	//禁止拷贝
	LoaderManager(const LoaderManager&) = delete;
	LoaderManager& operator=(const LoaderManager&) = delete;

public:
	void UpdateAssetFromDisk();
	bool LoadTextureFromFile(const std::string& path, std::shared_ptr<Texture>& tex);
	bool LoadShader(const std::string& path, std::shared_ptr<Shader>& shader,bool reload = false);
	bool LoadModel(const std::string& path, std::shared_ptr<Model>& model);
private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::string ReadAndPerprocessShaderFile(const std::string& path);
private:
	LoaderManager();
	std::unordered_map<std::string, FileState<Shader>> m_shaderFiles;
	std::unordered_map<std::string, FileState<Texture>> m_TextureFiles;
	std::unordered_map<std::string, FileState<Model>> m_model;
	std::shared_ptr<Shader> currentShader;
	std::shared_ptr<Model> currentModel;
};

