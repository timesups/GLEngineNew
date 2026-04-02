#pragma once
#include <vector>

#include "ShaderPass.h"




class Shader
{
public:
	Shader() 
	{

	};
	void CompileShaderFromCode(const std::vector<PassCode>& codes,const std::vector<PassOption>& options) 
	{
		//清除所有的pass
		m_passes.clear();
		for (int i = 0; i < codes.size(); i++) 
		{
			std::unique_ptr<ShaderPass> pass = std::make_unique<ShaderPass>(codes[i], options[i]);
			m_passes.push_back(std::move(pass));
		}
	}
	
public:
	std::vector<std::unique_ptr<ShaderPass>> m_passes;
	std::string m_name;
	std::string m_path;
};