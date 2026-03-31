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
			//如果Entity没有Transform自动创建一个
			m_transform = GetEntity()->AddComponent<Transform>();
		}
		//设置相机参数
		UpdateViewMatrix();
		UpdateProjectionMatrix();
	}
	virtual void Update(float deltaTime) override
	{
		if(m_transform)
		{
			//使用一个脏标志来标记矩阵是否需要更新，在渲染时进行计算.
			//因为矩阵变换是非常频繁的位置变化回调，每一帧都可能触发，造成帧间比较大的性能开销.
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

	float GetNear()const
	{
		return m_near;
	}
	float GetFar()const
	{
		return  m_far;
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
	//视图矩阵
	glm::mat4 mView = glm::mat4(1);
	glm::mat4 mProjection = glm::mat4(1);
	bool m_viewDirty = false;
};

