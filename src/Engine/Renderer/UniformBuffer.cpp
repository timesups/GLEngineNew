#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(size_t sizeBytes, GLenum usage)
{
	Create(sizeBytes, usage);
}

UniformBuffer::~UniformBuffer()
{
	Destroy();
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
	: m_id(other.m_id), m_size(other.m_size), m_usage(other.m_usage)
{
	other.m_id = 0;
	other.m_size = 0;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept
{
	if (this != &other)
	{
		Destroy();
		m_id = other.m_id;
		m_size = other.m_size;
		m_usage = other.m_usage;
		other.m_id = 0;
		other.m_size = 0;
	}
	return *this;
}

void UniformBuffer::Create(size_t sizeBytes, GLenum usage)
{
	Destroy();
	if (sizeBytes == 0)
		return;
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_UNIFORM_BUFFER, m_id);
	glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(sizeBytes), nullptr, usage);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	m_size = sizeBytes;
	m_usage = usage;
}

void UniformBuffer::Destroy()
{
	if (m_id != 0)
	{
		glDeleteBuffers(1, &m_id);
		m_id = 0;
		m_size = 0;
	}
}

void UniformBuffer::BindBase(GLuint uniformBlockBinding) const
{
	if (m_id != 0)
		glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBinding, m_id);
}

void UniformBuffer::Upload(const void* data, size_t size, size_t offset)
{
	if (!data || m_id == 0 || size == 0)
		return;
	if (offset + size > m_size)
		return;
	glBindBuffer(GL_UNIFORM_BUFFER, m_id);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
