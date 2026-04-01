#pragma once
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <string>


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
	glm::vec3 Tangent;
	glm::vec2 Texcoord;
};




class Mesh 
{
public:
	Mesh() {}
	~Mesh() 
	{
		if (m_vao) glDeleteVertexArrays(1, &m_vao);
		if (m_vbo) glDeleteBuffers(1, &m_vbo);
		if (m_ebo) glDeleteBuffers(1, &m_ebo);
	}
	void Create(const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices) 
	{
		m_vertexCount = vertices.size();
		m_indexCount = indices.size();
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		
		glGenBuffers(1, &m_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		//������
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);//����λ��

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,Normal));//���㷨��

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));//������ɫ

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));//������ɫ

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Texcoord));//������ɫ
		
		
		glBindVertexArray(0);

	}
	void Draw() const 
	{
		if (m_indexCount > 0) 
		{
			glBindVertexArray(m_vao);
			glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);
		}
	}
	size_t GetVertexCount()const { return m_vertexCount;}
	size_t GetIndexCount() const { return m_indexCount; }
public:
	std::string m_name;
private:
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ebo = 0;
	size_t m_vertexCount = 0;
	size_t m_indexCount = 0;
};


