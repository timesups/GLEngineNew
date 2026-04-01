#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Component.hpp"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/matrix.hpp"


class Transform : public Component
{
public:
	Transform() = default;

	//��ȡ����
	const glm::vec3& GetPosition() const 
	{
		return m_position;
	}
	const glm::vec3& GetRotation() const
	{
		return m_rotation;
	}
	const glm::vec3& GetScale() const
	{
		return m_scale;
	}
	const glm::vec3& GetFrontVector() const
	{
		return m_frontVector;
	}
	const glm::vec3& GetUpVector() const
	{
		return m_upVector;
	}
	glm::vec3 GetRightVector() const
	{
		return glm::normalize(glm::cross(m_frontVector, m_upVector));
	}

	/// 自由相机：rotation.x 俯仰、rotation.y 偏航（度），与 GetViewForward 一致
	glm::vec3 GetViewForward() const
	{
		const float yaw = glm::radians(m_rotation.y);
		const float pitch = glm::radians(m_rotation.x);
		glm::vec3 f;
		f.x = glm::sin(yaw) * glm::cos(pitch);
		f.y = glm::sin(pitch);
		f.z = -glm::cos(yaw) * glm::cos(pitch);
		return glm::normalize(f);
	}
	glm::vec3 GetViewRight() const
	{
		const glm::vec3 f = GetViewForward();
		glm::vec3 r = glm::cross(f, glm::vec3(0.f, 1.f, 0.f));
		const float len = glm::length(r);
		if (len < 1e-6f)
			return glm::vec3(1.f, 0.f, 0.f);
		return r / len;
	}
	glm::vec3 GetViewUp() const
	{
		return glm::normalize(glm::cross(GetViewRight(), GetViewForward()));
	}
	//��������
	void SetPosition(const glm::vec3& value) 
	{
		m_position = value;
		UpdateMatrix();
	}
	void SetRotation(const glm::vec3& value)
	{
		m_rotation = value;
		UpdateMatrix();
	}	
	void SetScale(const glm::vec3& value)
	{
		m_scale = value;
		UpdateMatrix();
	}
	const glm::mat4& GetModelMatrix()const 
	{
		return mModel;
	}
	const glm::mat4 GetNormalMatrix()const
	{
		return glm::transpose(glm::inverse(glm::mat4(glm::mat3(mModel))));		
	}
	void AddPosition(float x,float y,float z)
	{
		m_position += glm::vec3(x,y,z);
	}
	void AddPosition(const glm::vec3& value)
	{
		m_position += value;
	}
	void AddRotation(float x,float y,float z)
	{
		m_rotation += glm::vec3(x,y,z);
	}
	void AddRotation(const glm::vec3& value)
	{
		m_rotation += value;
	}
private:
	void UpdateMatrix() 
	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, m_position);
		model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, m_scale);
		mModel = model;
	}
	glm::vec3 m_position = glm::vec3(0,0,0);
	glm::vec3 m_rotation = glm::vec3(0, 0, 0);
	glm::vec3 m_scale = glm::vec3(1,1,1);
	glm::vec3 m_frontVector = glm::vec3(0.0, 0.0, -1.0f);
	glm::vec3 m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 mModel = glm::mat4(1);
};