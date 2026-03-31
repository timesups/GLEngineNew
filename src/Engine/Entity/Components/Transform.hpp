#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Component.hpp"
#include "glm/matrix.hpp"


class Transform : public Component
{
public:
	Transform() = default;

	//��ȡ����
	const glm::vec3& GetPosition() const 
	{
		return Position;
	}
	const glm::vec3& GetRotation() const
	{
		return Rotation;
	}
	const glm::vec3& GetScale() const
	{
		return Scale;
	}
	const glm::vec3& GetFrontVector() const
	{
		return FrontVector;
	}
	const glm::vec3& GetUpVector() const
	{
		return UpVector;
	}
	glm::vec3 GetRightVector() const
	{
		return glm::normalize(glm::cross(FrontVector, UpVector));
	}
	//��������
	void SetPosition(const glm::vec3& value) 
	{
		Position = value;
		UpdateMatrix();
	}
	void SetRotation(const glm::vec3& value)
	{
		Rotation = value;
		UpdateMatrix();
	}	
	void SetScale(const glm::vec3& value)
	{
		Scale = value;
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
private:
	void UpdateMatrix() 
	{
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, Position);
		model = glm::rotate(model, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, Scale);
		mModel = model;
	}
	glm::vec3 Position = glm::vec3(0,0,0);
	glm::vec3 Rotation = glm::vec3(0, 0, 0);
	glm::vec3 Scale = glm::vec3(1,1,1);
	glm::vec3 FrontVector = glm::vec3(0.0, 0.0, -1.0f);
	glm::vec3 UpVector = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 mModel = glm::mat4(1);
};