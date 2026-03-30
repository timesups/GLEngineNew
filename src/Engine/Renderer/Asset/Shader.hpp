#pragma once
#include "ShaderPass.hpp"




class Shader
{
public:
	Shader() 
	{

	};
	void CompileShaderFromCode(const std::vector<PassCode>& codes,const std::vector<PassOption>& options) 
	{
		//清除原有的所有pass
		m_passes.clear();
		//逐pass加载
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