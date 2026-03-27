#pragma once
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../../../external/stb/stb_image.h"


#include "../Renderer/Asset/Texture.hpp"
#include "../Renderer/Asset/Shader.hpp"



#define MODULE "LoaderManager"


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

std::string ReadAndPerprocessShaderFile(const std::string& path,std::vector<std::string>& files)
{
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
				std::string includeFileName = rootFolder + "/" + line.substr(quotesStart+1, quotesEnd - quotesStart - 1);
				files.push_back(includeFileName);
				shaderCode = shaderCode + "\n" + ReadAndPerprocessShaderFile(includeFileName, files);
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
		//Log(MODULE, LogLevel::ERROR, "ERROR::SHADER::FILE_NOT_SUCCESSFUL::READ::\n{}", e.what());
	}
	return shaderCode;
}

class LoaderManager
{
public:
	static std::unique_ptr<Texture> LoadTextureFromFile(const char* path)
	{
		stbi_set_flip_vertically_on_load(true);//反转图像

		int width, height, channels;
		unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
		if (!data) 
		{
			Log(MODULE, LogLevel::ERROR, "failed to load texture:{}", path);
			return nullptr;
		}

		std::unique_ptr<Texture> tex = std::make_unique<Texture>();

		tex->CreateFromData(data, width, height, channels);
		stbi_image_free(data);//释放内存
		return tex;
	}

	static std::vector<std::string> loadshaderPasses(const std::string& path,std::vector<std::string>& files)
	{
		std::vector<std::string> passes;
		size_t lashSlashIndex = path.find_last_of("/");
		std::string shaderName = path.substr(lashSlashIndex + 1);


		std::string ShaderFileContent = ReadAndPerprocessShaderFile(path, files);
		std::string shaderCodeLower = ShaderFileContent;
		stringToLower(shaderCodeLower);
		int SubShaderStartPoint = shaderCodeLower.find("subshader");
		if (SubShaderStartPoint == std::string::npos) {
			std::cout << "####" << "Shader:" << path << " SubShader not exist" << "####" << std::endl;

			return passes;
		}
		std::string subShader = getBlockContent(ShaderFileContent, SubShaderStartPoint);
		int PassStartPoint = 0;
		int currentPassIndex = 0;
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
			passes.push_back(pass);
			PassStartPoint += (pass.size() + 4);
			currentPassIndex += 1;
		}
		return passes;
	}
};