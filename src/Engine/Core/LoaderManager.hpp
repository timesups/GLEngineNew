#pragma once
#include <memory>
#include <fstream>



#include "../../../external/stb/stb_image.h"
#include "../Renderer/Asset/Texture.hpp"
#include "../Renderer/Asset/Shader.hpp"
#include "../Renderer/Asset/Model.hpp"
#include "../Core/Log.hpp"


#define MODULE "LoaderManager"




class LoaderManager
{
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


	static std::string ReadAndPerprocessShaderFile(const std::string& path)
	{
		std::string rootFolder = path.substr(0, path.find_last_of("/"));
		std::ifstream shaderFile;

		std::string shaderCode;
		shaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		try
		{
			shaderFile.open(path);
			std::string line;
			while (std::getline(shaderFile,line))
			{
				if (line.find("#include") != -1) 
				{
					int quotesStart = line.find_first_of("\"");
					int quotesEnd = line.find_last_of("\"");

					std::string includeFileName = rootFolder + "/" + line.substr(quotesStart + , quotesEnd - quotesStart - 1);

				}

			}
		}
		catch (const std::exception&)
		{

		}

	}
	static std::unique_ptr<Shader> LoadShaderFromFile(const std::string& path) 
	{
		std::string shaderName = path.substr(path.find_last_of("/") + 1);

		


		std::unique_ptr<Shader> shader = std::make_unique<Shader>();
		return shader;
	}



};