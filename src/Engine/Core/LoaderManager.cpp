#include "LoaderManager.h"

#include "../Core/Log.h"
#include "../Renderer/Asset/Mesh.h"
#include "../Renderer/Asset/Model.h"
#include "../Renderer/Asset/Shader.h"
#include "../Renderer/Asset/Texture.h"

#include "../../../external/stb/stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>

static constexpr const char* kModule = "LoaderManager";



Func DeterminFuncByString(const std::string& state)
{
	if (state == "always")
		return Func::ALWAYS;
	else if (state == "never")
		return Func::NEVER;
	else if (state == "less")
		return Func::LESS;
	else if (state == "equal")
		return Func::EQUAL;
	else if (state == "lequal")
		return Func::LEQUAL;
	else if (state == "greater")
		return Func::GREATER;
	else if (state == "notequal")
		return Func::NOTEQUAL;
	else if (state == "gequal")
		return Func::GEQUAL;

	return Func::ALWAYS;
}

StencilOp DeterminOpByString(const std::string& state)
{
	if(state == "keep")
		return StencilOp::KEEP;
	else if(state == "zero")
		return StencilOp::ZERO;
	else if(state == "replace")
		return StencilOp::REPLACE;
	else if(state == "incr")
		return StencilOp::INCR;
	else if(state == "incrwrap")
		return StencilOp::INCR_WRAP;
	else if(state == "decr")
		return StencilOp::DECR;
	else if(state == "decrwrap")
		return StencilOp::DECR_WRAP;
	else if(state == "invert")
		return StencilOp::INVERT;
	return StencilOp::KEEP;
}

BlendFunc DeterminBlendFunc(const std::string& state) 
{
	if (state == "zero")
		return BlendFunc::ZERO;
	else if (state == "one")
		return BlendFunc::ONE;
	else if (state == "srccolor")
		return BlendFunc::SRC_COLOR;
	else if (state == "oneminuscrccolor")
		return BlendFunc::ONE_MINUS_SRC_COLOR;
	else if (state == "dstcolor")
		return BlendFunc::DST_COLOR;
	else if (state == "oneminusdstcolor")
		return BlendFunc::ONE_MINUS_DST_COLOR;
	else if (state == "srcalpha")
		return BlendFunc::SRC_ALPHA;
	else if (state == "oneminussrcalpha")
		return BlendFunc::ONE_MINUS_SRC_ALPHA;
	else if (state == "dstalpha")
		return BlendFunc::DST_ALPHA;
	else if (state == "oneminusdstalpha")
		return BlendFunc::ONE_MINUS_DST_ALPHA;
	return BlendFunc::ZERO;
}

BlendEq DeterminBlendEquation(const std::string& state) 
{
	if (state == "add")
		return BlendEq::ADD;
	else if (state == "subtract")
		return BlendEq::SUBTRACT;
	else if (state == "reversesubtract")
		return BlendEq::REVERSE_SUBTRACT;
	else if (state == "min")
		return BlendEq::MIN;
	else if (state == "max")
		return BlendEq::MAX;
	return BlendEq::ADD;
}



LoaderManager::LoaderManager()
{
	//初始化一些文件相关的函数
	stbi_set_flip_vertically_on_load(true);//翻转图片
}

void LoaderManager::UpdateAssetFromDisk()
{
	std::vector<std::string> dirtyPaths;
	dirtyPaths.reserve(m_shaderFiles.size());
	for (auto& [path, file] : m_shaderFiles)
	{
		if (file.IsNeedReload())
			dirtyPaths.push_back(path);
	}
	if (dirtyPaths.empty())
		return;

	Log(kModule, LogLevel::INFO, "Shader hot-reload: {} file(s) changed", dirtyPaths.size());

	std::unordered_set<Shader*> seen;
	std::vector<std::shared_ptr<Shader>> uniqueShaders;
	for (const std::string& path : dirtyPaths)
	{
		auto it = m_shaderFiles.find(path);
		if (it == m_shaderFiles.end())
			continue;
		for (auto& shader : it->second.relativeAsset)
		{
			if (!shader)
				continue;
			if (seen.insert(shader.get()).second)
				uniqueShaders.push_back(shader);
		}
	}

	std::unordered_set<Shader*> failed;
	for (auto& shader : uniqueShaders)
	{
		if (!LoadShader(shader->m_path, shader, true))
		{
			failed.insert(shader.get());
			Log(kModule, LogLevel::ERROR, "Shader reload failed: {} ({})", shader->m_name, shader->m_path);
		}
		else
			Log(kModule, LogLevel::INFO, "Shader reload OK: {} ({})", shader->m_name, shader->m_path);
	}

	for (const std::string& path : dirtyPaths)
	{
		auto it = m_shaderFiles.find(path);
		if (it == m_shaderFiles.end())
			continue;
		FileState<Shader>& file = it->second;
		bool anyFailed = false;
		for (auto& shader : file.relativeAsset)
		{
			if (shader && failed.count(shader.get()))
			{
				anyFailed = true;
				break;
			}
		}
		if (!anyFailed)
			file.UpdateModifyTime();
	}
}

bool LoaderManager::LoadTextureFromFile(const std::string& path, std::shared_ptr<Texture>& tex)
{
	//检查路径是否已经加载
	if (m_TextureFiles.find(path) != m_TextureFiles.end())
	{
		//一个纹理只对应一个纹理资源，如果已经加载，那么取列表中的第一个纹理;
		tex = m_TextureFiles[path].relativeAsset[0];
		return true;
	}

	int width, height, channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		Log(kModule, LogLevel::ERROR, "failed to load texture:{}", path);
		return false;
	}

	tex->CreateFromData(data, width, height, channels);
	tex->m_path = path;
	tex->m_name = path.substr(path.find_last_of("/") + 1, path.find_last_of(".") - path.find_last_of("/") - 1);

	stbi_image_free(data);//释放内存

	FileState<Texture> file(path);
	file.AddRelativeAsset(tex);
	m_TextureFiles[path] = file;
	Log(kModule, LogLevel::INFO, "Texture loaded: {} ({}x{}, {} ch)", path, width, height, channels);
	return true;
}

bool LoaderManager::LoadShader(const std::string& path, std::shared_ptr<Shader>& shader,bool reload)
{
	//检查路径是否已经加载
	if (m_shaderFiles.find(path) != m_shaderFiles.end() && !reload)
	{
		shader = m_shaderFiles[path].relativeAsset[0];
		return true;
	}
	Log(kModule, LogLevel::INFO, "{}{}", reload ? "Reloading shader: " : "Loading shader: ", path);
	shader->m_name = path.substr(path.find_last_of("/") + 1, path.find_last_of(".") - path.find_last_of("/") - 1);
	shader->m_path = path;
	currentShader = shader;

	std::string ShaderFileContent = ReadAndPerprocessShaderFile(path);
	std::string shaderCodeLower = ShaderFileContent;
	stringToLower(shaderCodeLower);
	int SubShaderStartPoint = static_cast<int>(shaderCodeLower.find("subshader"));
	if (SubShaderStartPoint == std::string::npos)
	{
		Log(kModule, LogLevel::ERROR, "Shader '{}' has no SubShader block", path);
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
		PassStartPoint = static_cast<int>(passCodeLower.find("pass\n"));
		if (PassStartPoint == std::string::npos)
			break;

		std::string pass = getBlockContent(subShader, PassStartPoint);
		PassCode code;
		PassOption option;
		std::string realPassCode;
		//获取通道代码，然后替换
		std::istringstream iss(pass);
		std::string line;
		bool realCodeFlag = false;
		bool isInStencilBlock = false;
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
				int nameStart = static_cast<int>(lineLower.find("\"")) + 1;
				int nameEnd = static_cast<int>(lineLower.rfind("\""));
				code.passName = line.substr(nameStart, nameEnd - nameStart);
			}
			else if (lineLower.find("zwrite") != std::string::npos)
			{
				int start = lineLower.find("zwrite");
				std::string state = lineLower.substr(start + 7);
				if (state == "off")
					option.ZWrite = false;
				else
					option.ZWrite = true;
			}
			else if (lineLower.find("zfunc") != std::string::npos)
			{
				int start = lineLower.find("zfunc");
				std::string state = lineLower.substr(start + 6);
				option.zTest = DeterminFuncByString(state);

			}
			else if (lineLower.find("cull") != std::string::npos)
			{
				int start = lineLower.find("cull");
				std::string state = lineLower.substr(start + 5);
				if (state == "front")
					option.cullMode = CullMode::FRONT;
				else if (state == "back")
					option.cullMode = CullMode::BACK;
				else if (state == "off")
					option.cullMode = CullMode::OFF;
			}
			else if (lineLower.find("blend") != std::string::npos)
			{
				option.enableBlend = true;
				int space = lineLower.find("blend ");
				std::string state = lineLower.substr(space+6);
				space = state.find(" ");
				option.sFactor = DeterminBlendFunc(state.substr(0,space));
				option.dFactor = DeterminBlendFunc(state.substr(space+1));
			}
			else if (lineLower.find("blendeq") != std::string::npos)
			{
				int start = lineLower.find("blendeq");
				std::string state = lineLower.substr(start + 8);
				option.blendEq = DeterminBlendEquation(state);
			}
			else if(lineLower.find("stencil") != std::string::npos)
			{
				isInStencilBlock = true;
				continue;
			}
			
			if(isInStencilBlock)
			{
				if(lineLower.find("{") != std::string::npos)
				{
					continue;
				}
				else if(lineLower.find("mask") != std::string::npos)
				{
					int start = lineLower.find("mask");
					std::string state = lineLower.substr(start + 5);
					option.StencilMask = std::stoi(state,nullptr,0);
					continue;
				}
				else if(lineLower.find("func") != std::string::npos)
				{
					int start = lineLower.find("func");
					std::string state = lineLower.substr(start + 5);
					option.StencilFunc = DeterminFuncByString(state);
					continue;
				}
				else if(lineLower.find("ref") != std::string::npos)
				{
					int start = lineLower.find("ref");
					std::string state = lineLower.substr(start + 4);
					option.Stencilref = std::stoi(state,nullptr,0);
					continue;
				}
				else if(lineLower.find("fail") != std::string::npos)
				{
					int start = lineLower.find("fail");
					std::string state = lineLower.substr(start + 5);
					option.stencilFail = DeterminOpByString(state);
					continue;
				}
				else if(lineLower.find("dpfail") != std::string::npos)
				{
					int start = lineLower.find("dpfail");
					std::string state = lineLower.substr(start + 7);
					option.stencilDpFail = DeterminOpByString(state);
				}
				else if(lineLower.find("dppass") != std::string::npos)
				{
					int start = lineLower.find("dppass");
					std::string state = lineLower.substr(start + 7);
					option.stencilDpPass = DeterminOpByString(state);
				}
				else if (lineLower.find("}") != std::string::npos)
				{
					isInStencilBlock = false;
				}
			}

			if (iss.eof())
				break;
		}
		code.VertexShader = "#version 420 core\n#define VERTEX\n" + realPassCode;
		code.FragmentShader = "#version 420 core\n#define FRAGMENT\n" + realPassCode;
		if (passCode.find("#ifdef GEOMETRY") != static_cast<size_t>(-1))
		{
			code.FragmentShader = "#version 420 core\n#define GEOMETRY\n" + realPassCode;
		}
		codes.push_back(code);
		options.push_back(option);
		PassStartPoint += static_cast<int>(pass.size() + 4);
		currentPassIndex += 1;
	}
	(void)currentPassIndex;

	if (codes.empty())
	{
		Log(kModule, LogLevel::ERROR, "Shader '{}' has no Pass blocks", path);
		return false;
	}

	shader->CompileShaderFromCode(codes, options);
	return true;
}

bool LoaderManager::LoadModel(const std::string& path, std::shared_ptr<Model>& model)
{
	currentModel = model;
	currentModel->m_name = path.substr(path.find_last_of("/") + 1, path.find_last_of(".") - path.find_last_of("/") - 1);
	currentModel->m_path = path;

	Assimp::Importer importer;
	//如果没有顶点的法线模型处理中，我们会为模型生成法线和UV坐标
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Log(kModule, LogLevel::ERROR, "Assimp failed '{}': {}", path, importer.GetErrorString());
		return false;
	}
	ProcessNode(scene->mRootNode, scene);
	Log(kModule, LogLevel::INFO, "Model '{}' loaded ({} mesh sections)", model->m_name, model->m_meshSections.size());
	return true;
}

void LoaderManager::ProcessNode(aiNode* node, const aiScene* scene)
{
	//处理所有的网格
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene);
	}
	//递归处理节点
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}

void LoaderManager::ProcessMesh(aiMesh* mesh, const aiScene* /*scene*/)
{
	std::shared_ptr<Mesh> out_mesh = std::make_unique<Mesh>();
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//处理顶点信息
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
			vector.x = mesh->mColors[0][i].r;
			vector.y = mesh->mColors[0][i].g;
			vector.z = mesh->mColors[0][i].b;
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
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.Texcoord = vec;
		}
		else
		{
			vertex.Texcoord = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	//处理索引信息
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	out_mesh->Create(vertices, indices);
	out_mesh->m_name = mesh->mName.C_Str();
	//添加到模型
	currentModel->AddSection(out_mesh);
}

std::string LoaderManager::ReadAndPerprocessShaderFile(const std::string& path)
{
	if (m_shaderFiles.find(path) == m_shaderFiles.end())
	{
		m_shaderFiles[path] = FileState<Shader>(path);
	}
	m_shaderFiles[path].AddRelativeAsset(currentShader);

	std::string rootFolder = path.substr(0, path.find_last_of("/"));
	std::ifstream shaderFile;

	std::string shaderCode;
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		shaderFile.open(path);
		if (!shaderFile.is_open())
		{
			Log(kModule, LogLevel::ERROR, "Cannot open shader file: {}", path);
			return shaderCode;
		}
		std::string line;
		while (std::getline(shaderFile, line))
		{
			if (line.find("#include") != static_cast<size_t>(-1))
			{
				int quotesStart = static_cast<int>(line.find_first_of("\""));
				int quotesEnd = static_cast<int>(line.find_last_of("\""));
				std::string includeFileName = rootFolder + "/" + line.substr(quotesStart + 1, quotesEnd - quotesStart - 1);
				shaderCode = shaderCode + "\n" + ReadAndPerprocessShaderFile(includeFileName);
			}
			else
			{
				shaderCode = shaderCode + "\n" + line;
			}
			if (shaderFile.eof())
				break;
		}
	}
	catch (const std::ifstream::failure& e)
	{
		Log(kModule, LogLevel::ERROR, "ERROR::SHADER::FILE_NOT_SUCCESSFUL::READ::\n{}", e.what());
	}
	return shaderCode;
}

