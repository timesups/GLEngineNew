#pragma once
#include <glad/glad.h>

#include "../../Core/Log.hpp"

#define MODULE "Texture"

class Texture 
{
public:
	Texture() :m_width(0), m_height(0), m_channels(0) 
	{
		glGenTextures(GL_TEXTURE_2D, &m_id);
	}
	~Texture() {if (m_id) glDeleteTextures(1, &m_id);}//清理资源
	bool CreateFromData(unsigned char* data, int width, int height, int channels) 
	{
		glBindTexture(GL_TEXTURE_2D, m_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		GLenum format = GL_RGB;
		if (channels > 3) format = GL_RGBA;	
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	bool CreateFromEmpty(int width, int height, int channels) 
	{
		glBindTexture(GL_TEXTURE_2D, m_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		GLenum format = GL_RGB;
		if (channels > 3) format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void Bind(unsigned int uint = 0)  
	{
		if (!m_id) 
		{
			Log(MODULE, LogLevel::ERROR, "Try to bind a invalid texture");
			return;
		}
		if(m_isBound && m_lastBoundUnit == uint)
		{
			return;
		}
		glActiveTexture(GL_TEXTURE0 + uint);
		glBindTexture(GL_TEXTURE_2D, m_id);

		m_lastBoundUnit = uint;
		m_isBound = true;

	}
	void UnBind()  
	{
		if (!m_isBound) return;
		glActiveTexture(GL_TEXTURE0 + m_lastBoundUnit);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		m_isBound = false;
	}
public:
	std::string m_path;
	std::string m_name;
private:
	unsigned int m_id;
	int m_width, m_height, m_channels;

	bool m_isBound = false;
	unsigned int m_lastBoundUnit = 0;
};
