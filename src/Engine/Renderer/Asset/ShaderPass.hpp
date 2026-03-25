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

void CheckShaderCompileState(unsigned int ID, SHADERTYPE type);

class ShaderPass
{
public:
	ShaderPass() :m_id(0) {}
	~ShaderPass() { if (m_id != 0) glDeleteProgram(m_id); }

	//Ω˚÷πøΩ±¥
	ShaderPass(const ShaderPass&) = delete;
	ShaderPass& operator=(const ShaderPass&) = delete;

	//“∆∂Ø”Ô“Â
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
private:
	unsigned int m_id;
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