#pragma once
#include "ShaderPass.hpp"

class Shader
{
public:
	Shader() 
	{

	};
	void CompileShaderFromCode(const std::string& code) 
	{
		//清除原有的所有pass
		m_passes.clear();





	}
public:
	std::vector<std::unique_ptr<ShaderPass>> m_passes;
	std::string m_name;
};