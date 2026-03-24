#pragma once
#include "Transform.hpp"
#include "../Entity.hpp"



enum class MOVEDIRECTION
{
	FORWARD,
	BACKWARD,
	RIGHT,
	LEFT,
	UP,
	DOWN,
};

class Camera:public Component
{
public:
	Camera() = default;

	virtual void Init() override 
	{
		m_transform = GetEntity()->GetComponent<Transform>();
		if (!m_transform) {
			//如果父级Entity没有Transform自动添加一个
			m_transform = GetEntity()->AddComponent<Transform>();
		}
		//更新相机矩阵
		UpdateViewMatrix();
		UpdateProjectionMatrix();
	}
	virtual void Update(float deltaTime) override
	{
		if(m_transform)
		{
			//使用一个变量标记视图矩阵 需要更新,在渲染的时候在计算.
			//因为试图矩阵是和自身的位置变化相关的,所以每一帧都有可能更新,但是逐帧更新会比较消耗资源.
			m_viewDirty = true;
		}
	}
	const glm::mat4& GetViewMatrix() 
	{
		if (m_viewDirty && m_transform) 
		{
			UpdateViewMatrix();
			m_viewDirty = false;
		}
		return mView;
	}
	const glm::mat4& GetProjectionMatrix()const 
	{
		return mProjection;
	}

	glm::vec3 GetPosition() const 
	{
		return m_transform ? m_transform->GetPosition() : glm::vec3(0);
	}
	glm::vec3 GetRightVector() const 
	{
		return m_transform ? m_transform->GetRightVector() : glm::vec3(1.0,0.0,0.0);

	}

private:
	void UpdateViewMatrix() 
	{
		if (!m_transform)
			return;
		glm::vec3 pos = m_transform->GetPosition();
		glm::vec3 forward = m_transform->GetFrontVector();
		glm::vec3 up = m_transform->GetUpVector();

		mView = glm::lookAt(pos, pos + forward, up);
	}
	void UpdateProjectionMatrix() 
	{
		mProjection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
	}
	Transform* m_transform = nullptr;
	//投影参数
	float m_fov = 60.f;
	float m_aspect = 16.0f / 9.0f;
	float m_near = 0.1f;
	float m_far = 1000.0f;
	//矩阵缓存
	glm::mat4 mView = glm::mat4(1);
	glm::mat4 mProjection = glm::mat4(1);
	bool m_viewDirty = false;
};

