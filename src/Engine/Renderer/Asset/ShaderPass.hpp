#pragma once
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Core/Log.hpp"


#define MODULE "Shader"

enum SHADERTYPE
{
	VERTEX,
	FRAGMENT,
	GEOMETRY,
	PROGRAM,
};

enum class CullMode
{
	BACK,//默认剔除背面
	FRONT,//剔除正面
	OFF,//双面渲染
};


enum class ZTEST 
{
	ALWAYS = GL_ALWAYS,     //永远通过深度测试  等于关闭深度测试
	NEVER = GL_NEVER,       //永远不通过深度测试
	LESS = GL_LESS,         //片段深度值小于深度缓冲时通过测试
	EQUAL = GL_EQUAL,       //片段深度值等于深度缓冲时通过测试
	LEQUAL = GL_LEQUAL,     //片段深度值小于等于深度缓冲时通过测试
	GREATER = GL_GREATER,   //片段深度值大于深度缓冲时通过测试
	NOTEQUAL = GL_NOTEQUAL, //片段深度值不等于深度缓冲时通过测试
	GEQUAL = GL_GEQUAL,     //片段深度值大于等于深度缓冲时通过测试
};



struct ShaderPassOptions
{
	ZTEST zTest = ZTEST::LESS;
}; 

void CheckShaderCompileState(unsigned int ID, SHADERTYPE type);

class ShaderPass
{
public:
	ShaderPass() :m_id(0) {}
	~ShaderPass() { if (m_id != 0) glDeleteProgram(m_id); }

	//禁止拷贝
	ShaderPass(const ShaderPass&) = delete;
	ShaderPass& operator=(const ShaderPass&) = delete;

	//移动语义
	ShaderPass(ShaderPass&& other) noexcept;
	ShaderPass& operator=(ShaderPass&& other) noexcept;


	void LoadFromCode(const char* vs, const char* fs)
	{
		unsigned int vid, fid;
		vid = glCreateShader(GL_VERTEX_SHADER);
		fid = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vid, 1, &vs, NULL);
		glShaderSource(fid, 1, &fs, NULL);

		glCompileShader(vid);
		CheckShaderCompileState(vid, VERTEX);
		glCompileShader(fid);
		CheckShaderCompileState(fid, FRAGMENT);

		m_id = glCreateProgram();
		glAttachShader(m_id, vid);
		glAttachShader(m_id, fid);
		glLinkProgram(m_id);
		CheckShaderCompileState(m_id, PROGRAM);

		glDeleteShader(vid);
		glDeleteShader(fid);
	}


	void LoadFromCode(const char* vs, const char* fs, const char* gs)
	{
		unsigned int vid, fid, gid;
		vid = glCreateShader(GL_VERTEX_SHADER);
		fid = glCreateShader(GL_FRAGMENT_SHADER);
		gid = glCreateShader(GL_GEOMETRY_SHADER);

		glShaderSource(vid, 1, &vs, NULL);
		glShaderSource(fid, 1, &fs, NULL);
		glShaderSource(gid, 1, &gs, NULL);

		glCompileShader(vid);
		CheckShaderCompileState(vid, VERTEX);
		glCompileShader(fid);
		CheckShaderCompileState(fid, FRAGMENT);
		glCompileShader(gid);
		CheckShaderCompileState(gid, GEOMETRY);


		m_id = glCreateProgram();
		glAttachShader(m_id, vid);
		glAttachShader(m_id, fid);
		glAttachShader(m_id, gid);
		glLinkProgram(m_id);
		CheckShaderCompileState(m_id, PROGRAM);

		glDeleteShader(vid);
		glDeleteShader(fid);
		glDeleteShader(gid);
	}

	void use()
	{
		glUseProgram(m_id);
	}
	void SetValue(const std::string& name, glm::vec3 value) const
	{
		glUniform3f(glGetUniformLocation(m_id, name.c_str()), value.x, value.y, value.z);
	}
	void SetValue(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
	}
	void SetValue(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
	}
	void SetValue(const std::string& name, glm::vec4 value) const
	{
		glUniform4f(glGetUniformLocation(m_id, name.c_str()), value.x, value.y, value.z, value.w);
	}
	void SetValue(const std::string& name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
	}
	void SetValue(const std::string& name, int value)const
	{
		glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
	}
	ShaderPassOptions& GetOptions() 
	{
		return m_options;
	}
	void SetOptions(ShaderPassOptions options) 
	{
		m_options = options;
	}
	void SetName(const std::string& name)
	{
		m_name = name;
	}
	const std::string& GetName()
	{
		return m_name;
	}
private:
	unsigned int m_id;
	ShaderPassOptions m_options;
	std::string m_name;
};

void CheckShaderCompileState(unsigned int ID, SHADERTYPE type)
{
	int success;
	char infoLog[512];

	if (type == PROGRAM)
	{
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			Log(MODULE, LogLevel::ERROR, "ERROR:SHADER::PROGRAM::LINK_FAIED\n{}", infoLog);
		}
	}
	else
	{
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(ID, 512, NULL, infoLog);
			switch (type)
			{
			case VERTEX:
				Log(MODULE, LogLevel::ERROR, "ERROR::SHADER::VERTEX::COMPIATION_FAILED\n{}", infoLog);
				break;
			case FRAGMENT:
				Log(MODULE, LogLevel::ERROR, "ERROR::SHADER::FRAGMENT::COMPIATION_FAILED\n{}", infoLog);
				break;
			case GEOMETRY:
				Log(MODULE, LogLevel::ERROR, "ERROR::SHADER::GEOMETRY::COMPIATION_FAILED\n{}", infoLog);
				break;
			default:
				break;
			}
		}
	}


}