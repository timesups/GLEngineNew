#pragma once
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "../../../external/stb/stb_image.h"
#include "../Renderer/Asset/Texture.hpp"
#include "../Renderer/Asset/Shader.hpp"
#include "../Renderer/Asset/Model.hpp"


#define MODULE "LoaderManager"


int64_t get_mtime(const std::string& filePath)
{
	struct stat result;
	if (stat(filePath.c_str(), &result) == 0) {
		auto mod_time = result.st_mtime;
		return mod_time;
	}
	return -1;
}

void stringToLower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](char& c) {
		return std::tolower(c); });
}

std::string getBlockContent(const std::string& content, const int startPoint)
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

//utility
template<typename T>
struct FileState
{
	FileState() = default;
	FileState(const std::string&path,std::shared_ptr<T> asset) 
	{
		this->path = path;
		relativeAsset = asset;
		UpdateModifyTime();
	}
	void UpdateModifyTime() 
	{
		mTime = get_mtime(path);
	}
	std::string path;
	std::shared_ptr<T> relativeAsset;
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
	static LoaderManager* Get()
	{
		if (!instance)
			instance = new LoaderManager();
		return instance;
	}

	~LoaderManager() 
	{
		if (instance) 
		{
			delete instance;
			instance = nullptr;
		}
	}
public:
	void UpdateAssetFromDisk() 
	{
		for (auto& file : m_shaderFiles) 
		{
			if (file.second.IsNeedReload()) 
			{
				//重新加载shader
				LoadShader(file.second.relativeAsset->m_path, file.second.relativeAsset);
				//更新时间
				file.second.UpdateModifyTime();
			}
		}
	}
	bool LoadTextureFromFile(const std::string &path, std::shared_ptr<Texture> tex)
	{
		//首先判断该路径是否已经被加载
		if (m_TextureFiles.find(path)!=m_TextureFiles.end())
		{
			tex = m_TextureFiles[path].relativeAsset;
			return true;
		}

		int width, height, channels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data)
		{
			Log(MODULE, LogLevel::ERROR, "failed to load texture:{}", path);
			return false;
		}

		tex->CreateFromData(data, width, height, channels);
		tex->m_path = path;
		tex->m_name = path.substr(path.find_last_of("/") + 1);

		stbi_image_free(data);//释放内存

		FileState<Texture> file(path,tex);
		m_TextureFiles[path] = file;
		return true;
	}

	bool LoadShader(const std::string& path, std::shared_ptr<Shader> shader)
	{
		Log(MODULE, LogLevel::INFO, "Loading shader from path:{}", path);
		shader->m_name = path.substr(path.find_last_of("/") + 1);
		shader->m_path = path;
		std::vector<std::string> passes;
		currentShader = shader;
		std::string ShaderFileContent = ReadAndPerprocessShaderFile(path);
		std::string shaderCodeLower = ShaderFileContent;
		stringToLower(shaderCodeLower);
		int SubShaderStartPoint = shaderCodeLower.find("subshader");
		if (SubShaderStartPoint == std::string::npos) {
			std::cout << "####" << "Shader:" << path << " SubShader not exist" << "####" << std::endl;
			return false;
		}
		std::string subShader = getBlockContent(ShaderFileContent, SubShaderStartPoint);
		int PassStartPoint = 0;
		int currentPassIndex = 0;
		std::vector<PassCode> codes;
		std::vector<PassOption> options;
		while (true)
		{
			std::string passCode = subShader.substr(PassStartPoint);
			std::string passCodeLower = passCode;
			stringToLower(passCodeLower);
			subShader = passCode;
			PassStartPoint = passCodeLower.find("pass\n");
			if (PassStartPoint == std::string::npos)
				break;
			std::string pass = getBlockContent(subShader, PassStartPoint);
			PassCode code;
			PassOption option;
			std::string realPassCode;
			//提取着色器代码,宏替换
			std::istringstream iss(pass);
			std::string line;
			bool realCodeFlag = false;
			while (std::getline(iss, line))
			{
				if (line.find("GLSLPROGRAM") != std::string::npos)
				{
					realCodeFlag = true;
					continue;
				}
				if (line.find("ENDGLSL") != std::string::npos)
				{
					realCodeFlag = false;
					continue;
				}
				if (realCodeFlag)
				{
					realPassCode = realPassCode + "\n" + line;
					continue;
				}
				std::string lineLower = line;
				stringToLower(lineLower);
				if (lineLower.find("name") != std::string::npos)
				{
					int nameStart = lineLower.find("\"") + 1;
					int nameEnd = lineLower.rfind("\"");
					code.passName = line.substr(nameStart, nameEnd - nameStart);
				}
				else if (lineLower.find("zwrite") != std::string::npos)
				{
					//int start = lineLower.find("zwrite");
					//std::string state = lineLower.substr(start + 7);
					//if (state == "off")
					//	zWrite = false;
					//else
					//	zWrite = true;
				}
				else if (lineLower.find("zfunc") != std::string::npos)
				{
					//int start = lineLower.find("zfunc");
					//std::string state = lineLower.substr(start + 6);
					//if (state == "always")
					//	zFunc = GL_ALWAYS;
					//else if (state == "never")
					//	zFunc = GL_NEVER;
					//else if (state == "less")
					//	zFunc = GL_LESS;
					//else if (state == "equal")
					//	zFunc = GL_EQUAL;
					//else if (state == "lequal")
					//	zFunc = GL_LEQUAL;
					//else if (state == "greater")
					//	zFunc = GL_GREATER;
					//else if (state == "notequal")
					//	zFunc = GL_NOTEQUAL;
					//else if (state == "gequal")
					//	zFunc = GL_GEQUAL;
				}
				else if (lineLower.find("cull") != std::string::npos)
				{
					//cull = true;
					//int start = lineLower.find("cull");
					//std::string state = lineLower.substr(start + 5);
					//if (state == "front")
					//	cullFace = GL_FRONT;
					//else if (state == "back")
					//	cullFace = GL_BACK;
				}
				else if (lineLower.find("blend") != std::string::npos)
				{
					//blend = true;
					//int start = lineLower.find("blend");
					//std::string state = lineLower.substr(start + 6);
					//int splitIndex = state.find(" ");
					//if (splitIndex == std::string::npos)
					//	continue;
					//std::string src = state.substr(0, splitIndex);
					//std::string dst = state.substr(splitIndex + 1);
					//setBlendFunc(sFactor, src);
					//setBlendFunc(dFactor, dst);
				}
				if (iss.eof())
					break;
			}
			code.VertexShader = "#version 420 core\n#define VERTEX\n" + realPassCode;
			code.FragmentShader = "#version 420 core\n#define FRAGMENT\n" + realPassCode;
			if (passCode.find("#ifdef GEOMETRY") != -1)
			{
				code.FragmentShader = "#version 420 core\n#define GEOMETRY\n" + realPassCode;
			}
			codes.push_back(code);
			options.push_back(option);
			PassStartPoint += (pass.size() + 4);
			currentPassIndex += 1;
		}
		shader->CompileShaderFromCode(codes, options);
		return true;
	}
	bool LoadModel(const std::string& path, std::shared_ptr<Model> model) 
	{
		currentModel = std::make_shared<Model>();
		currentModel->m_name = path.substr(path.find_last_of("/") + 1);
		currentModel->m_path = path;

		Assimp::Importer importer;
		//在这里对导入的模型进行后处理,三角化模型并且反转UV的v轴
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR:ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		ProcessNode(scene->mRootNode, scene);
	}
private:
	void ProcessNode(aiNode* node, const aiScene* scene)
	{
		//遍历所有的网格
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene);

		}
		//递归处理节点
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			ProcessNode(node->mChildren[i], scene);
	}
	void ProcessMesh(aiMesh* mesh, const aiScene* scene) 
	{
		std::unique_ptr<Mesh> out_mesh = std::make_unique<Mesh>();
		std::unique_ptr<Material> out_mat = std::make_unique<Material>();
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		//顶点信息
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			//位置
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			//法线
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			//顶点颜色
			if (mesh->mColors[0]) 
			{
				vector.x = mesh->mColors[0]->r;
				vector.y = mesh->mColors[0]->g;
				vector.z = mesh->mColors[0]->b;
				vertex.Color = vector;
			}
			else
			{
				vertex.Color = glm::vec3(0, 0, 0);
			}
			//纹理坐标
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0]->x;
				vec.y = mesh->mTextureCoords[0]->y;
				vertex.Texcoord = vec;
			}
			else
			{
				vertex.Texcoord = glm::vec2(0.0f, 0.0f);
			}
			vertices.push_back(vertex);
		}

		//索引信息
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		out_mesh->Create(vertices, indices);

		//材质贴图
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		currentModel->AddSection(out_mesh,);
	}
	std::string ReadAndPerprocessShaderFile(const std::string& path)
	{
		if (m_shaderFiles.find(path) == m_shaderFiles.end()) 
			m_shaderFiles[path] = FileState<Shader>(path, currentShader);

		std::string rootFolder = path.substr(0, path.find_last_of("/"));
		std::ifstream shaderFile;

		std::string shaderCode;
		shaderFile.exceptions(std::ifstream::badbit);
		try
		{
			shaderFile.open(path);
			std::string line;
			while (std::getline(shaderFile, line))
			{
				if (line.find("#include") != -1)
				{
					int quotesStart = line.find_first_of("\"");
					int quotesEnd = line.find_last_of("\"");
					std::string includeFileName = rootFolder + "/" + line.substr(quotesStart + 1, quotesEnd - quotesStart - 1);
					shaderCode = shaderCode + "\n" + ReadAndPerprocessShaderFile(includeFileName);
				}
				else
				{
					shaderCode = shaderCode + "\n" + line;
				}
				//正常情况下文件到达末尾会抛出failbit错误,这里检查文件是否到达末尾来忽略这个错误
				if (shaderFile.eof())
					break;

			}
		}
		catch (const std::ifstream::failure e)
		{
			Log(MODULE, LogLevel::ERROR, "ERROR::SHADER::FILE_NOT_SUCCESSFUL::READ::\n{}", e.what());
		}
		return shaderCode;
	}
private:
	static LoaderManager* instance;
	LoaderManager() 
	{
		//初始化一些文件加载的函数
		stbi_set_flip_vertically_on_load(true);//反转图像
	}
	std::unordered_map<std::string, FileState<Shader>> m_shaderFiles;
	std::unordered_map<std::string, FileState<Texture>> m_TextureFiles;
	std::unordered_map<std::string, FileState<Model>> m_model;
	std::shared_ptr<Shader> currentShader;
	std::shared_ptr<Model> currentModel;
};

LoaderManager* LoaderManager::instance = nullptr;