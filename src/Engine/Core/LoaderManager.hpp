#pragma once
#include <memory>


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

	static std::unique_ptr<Shader> LoadShaderFromFile(const char* path) 
	{

	}



};