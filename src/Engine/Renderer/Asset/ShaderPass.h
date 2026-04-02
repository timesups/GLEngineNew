#pragma once
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Core/Log.h"


static constexpr const char* kShaderPassModule = "Shader";

enum SHADERTYPE
{
	VERTEX,
	FRAGMENT,
	GEOMETRY,
	PROGRAM,
};

enum class CullMode
{
	BACK = GL_BACK,
	FRONT = GL_FRONT,
	OFF = -1,
};


// 与 glDepthFunc / glStencilFunc 的比较方式一致：深度为片段深度与深度缓冲比较，模板为 ref 与模板缓冲比较。
enum class Func
{
	ALWAYS = GL_ALWAYS,     // 始终通过比较
	NEVER = GL_NEVER,       // 始终不通过比较
	LESS = GL_LESS,         // 小于时通过（深度：片段深度 < 缓冲值；模板：ref < 缓冲值）
	EQUAL = GL_EQUAL,       // 相等时通过
	LEQUAL = GL_LEQUAL,     // 小于等于时通过
	GREATER = GL_GREATER,   // 大于时通过
	NOTEQUAL = GL_NOTEQUAL, // 不相等时通过
	GEQUAL = GL_GEQUAL,     // 大于等于时通过
};

enum class BlendFunc 
{
	ZERO                      =  GL_ZERO,
	ONE                       =  GL_ONE,
	SRC_COLOR                 =  GL_SRC_COLOR,
	ONE_MINUS_SRC_COLOR       =  GL_ONE_MINUS_SRC_COLOR,
	DST_COLOR                 =  GL_DST_COLOR,
	ONE_MINUS_DST_COLOR       =  GL_ONE_MINUS_DST_COLOR,
	SRC_ALPHA                 =  GL_SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA       =  GL_ONE_MINUS_SRC_ALPHA,
	DST_ALPHA                 =  GL_DST_ALPHA,
	ONE_MINUS_DST_ALPHA       =  GL_ONE_MINUS_DST_ALPHA,
};

enum class BlendEq
{
	ADD = GL_FUNC_ADD,
	SUBTRACT = GL_FUNC_SUBTRACT,
	REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT,
	MIN = GL_MIN,
	MAX = GL_MAX,
};

enum class StencilOp
{
	KEEP = GL_KEEP,           //保持当前
	ZERO = GL_ZERO,           //将模板设置为0
	REPLACE = GL_REPLACE,     //将模板值设置为glStencilFunc函数设置的ref值
	INCR = GL_INCR,           //如果模板值小于最大值则将模板值加1
	INCR_WRAP = GL_INCR_WRAP, //
	DECR = GL_DECR,
	DECR_WRAP = GL_DECR_WRAP,
	INVERT = GL_INVERT,
};

struct PassCode
{
	std::string VertexShader;
	std::string GeometryShader;
	std::string FragmentShader;
	std::string passName;
};


struct PassOption
{
	//深度测试
	Func zTest = Func::LESS;
	bool ZWrite = true;

	//面剔除
	CullMode cullMode = CullMode::OFF;

	//模板测试
	int StencilMask = 0;
	Func StencilFunc = Func::ALWAYS;
	int Stencilref = 1;
	StencilOp stencilFail = StencilOp::KEEP;//模板测试失败
	StencilOp stencilDpFail = StencilOp::KEEP;//模板通过,深度失败
	StencilOp stencilDpPass = StencilOp::KEEP;//模板深度都通过
	
	//混合
	bool enableBlend = false;
	BlendFunc sFactor = BlendFunc::ZERO;
	BlendFunc dFactor = BlendFunc::ZERO;
	BlendEq blendEq = BlendEq::ADD;
}; 

void CheckShaderCompileState(unsigned int ID, SHADERTYPE type);

class ShaderPass
{
public:
	ShaderPass(const PassCode &code, PassOption option = PassOption())
	{
		if (code.GeometryShader.empty())
			LoadFromCode(code.VertexShader.c_str(), code.FragmentShader.c_str());
		else
			LoadFromCode(code.VertexShader.c_str(), code.GeometryShader.c_str(), code.FragmentShader.c_str());
		m_name = code.passName;
		this->m_options = option;
	}
	~ShaderPass() { if (m_id != 0) glDeleteProgram(m_id); }

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
	void SetValue(const std::string& name, const glm::vec3& value) const
	{
		glUniform3f(glGetUniformLocation(m_id, name.c_str()), value.x, value.y, value.z);
	}
	void SetValue(const std::string& name, const glm::vec2& value) const
	{
		glUniform2f(glGetUniformLocation(m_id, name.c_str()), value.x, value.y);
	}
	void SetValue(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
	}
	void SetValue(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
	}
	void SetValue(const std::string& name, const glm::vec4& value) const
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
	void SetValue(const std::string& name, const glm::mat4& value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void SetValue(const std::string& name, const glm::mat3& value) const
	{
		glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	PassOption& GetOptions() 
	{
		return m_options;
	}
	void SetOptions(PassOption options) 
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
	void SetState() const
	{
		//深度测试
		glDepthMask((GLboolean)m_options.ZWrite);
		glDepthFunc((GLenum)m_options.zTest);
		//面剔除
		if(m_options.cullMode != CullMode::OFF)
		{
			glEnable(GL_CULL_FACE);
			glCullFace((GLenum)m_options.cullMode);
		}
		else{if(glIsEnabled(GL_CULL_FACE))glDisable(GL_CULL_FACE);}
		//模板测试
		if(m_options.StencilMask != 0)
		{
			glEnable(GL_STENCIL_TEST);
			glStencilFunc((GLenum)m_options.StencilFunc,m_options.Stencilref,0xff);
			glStencilOp((GLenum)m_options.stencilFail,(GLenum)m_options.stencilDpFail,(GLenum)m_options.stencilDpPass);
		}
		else{if(glIsEnabled(GL_STENCIL_TEST))glDisable(GL_STENCIL_TEST);}
		//混和
		if(m_options.enableBlend)
		{
			glEnable(GL_BLEND);
			glBlendFunc((GLenum)m_options.sFactor,(GLenum)m_options.dFactor);
			glBlendEquation((GLenum)m_options.blendEq);
		}
		else{if(glIsEnabled(GL_BLEND))glDisable(GL_BLEND);}
	}
private:
	unsigned int m_id;
	PassOption m_options;
	std::string m_name;
};

inline void CheckShaderCompileState(unsigned int ID, SHADERTYPE type)
{
	int success;
	char infoLog[512];

	if (type == PROGRAM)
	{
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			Log(kShaderPassModule, LogLevel::ERROR, "ERROR:SHADER::PROGRAM::LINK_FAIED\n{}", infoLog);
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
				Log(kShaderPassModule, LogLevel::ERROR, "ERROR::SHADER::VERTEX::COMPIATION_FAILED\n{}", infoLog);
				break;
			case FRAGMENT:
				Log(kShaderPassModule, LogLevel::ERROR, "ERROR::SHADER::FRAGMENT::COMPIATION_FAILED\n{}", infoLog);
				break;
			case GEOMETRY:
				Log(kShaderPassModule, LogLevel::ERROR, "ERROR::SHADER::GEOMETRY::COMPIATION_FAILED\n{}", infoLog);
				break;
			default:
				break;
			}
		}
	}


}