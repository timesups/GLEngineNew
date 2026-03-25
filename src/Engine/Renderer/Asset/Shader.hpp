#pragma once

#include "ShaderPass.hpp"

class Shader
{
public:
	Shader() = default;
public:
	std::vector<ShaderPass> passes;
};