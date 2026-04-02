#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Transform.h"
#include "../Entity.h"

class Camera : public Component
{
public:
	Camera() = default;

	virtual void Init() override
	{
		m_transform = GetEntity()->GetComponent<Transform>();
		if (!m_transform)
			m_transform = GetEntity()->AddComponent<Transform>();
		UpdateViewMatrix();
		UpdateProjectionMatrix();
	}

	virtual void Update(float deltaTime) override
	{
		if (!m_transform)
			return;

		if (m_pendingScrollDeltaY != 0.f)
		{
			m_fov += m_pendingScrollDeltaY * m_fovDegreesPerScroll;
			m_fov = glm::clamp(m_fov, m_fovMin, m_fovMax);
			m_pendingScrollDeltaY = 0.f;
			UpdateProjectionMatrix();
		}

		if (m_pendingMouseDx != 0.0 || m_pendingMouseDy != 0.0)
		{
			glm::vec3 rot = m_transform->GetRotation();
			rot.y += static_cast<float>(m_pendingMouseDx) * m_mouseSensitivity;
			rot.x -= static_cast<float>(m_pendingMouseDy) * m_mouseSensitivity;
			rot.x = glm::clamp(rot.x, m_pitchMin, m_pitchMax);
			m_transform->SetRotation(rot);
			m_pendingMouseDx = 0.0;
			m_pendingMouseDy = 0.0;
			m_viewDirty = true;
		}

		if (MoveFreeFly(deltaTime))
			m_viewDirty = true;
	}

	void QueueScrollDeltaY(float dy) { m_pendingScrollDeltaY += dy; }

	/// 本帧鼠标位移（像素），在 Update 中转为视角旋转
	void QueueMouseLookDelta(double dx, double dy)
	{
		m_pendingMouseDx += dx;
		m_pendingMouseDy += dy;
	}

	/// -1 / 0 / 1，分别对应前后、右左、世界上下（E/Q）
	void SetMoveAxes(float forward, float right, float worldUp) { m_axisForward = forward; m_axisRight = right; m_axisWorldUp = worldUp; }

	void SetSprint(bool sprint) { m_sprint = sprint; }

	const glm::mat4& GetViewMatrix()
	{
		if (m_viewDirty && m_transform)
		{
			UpdateViewMatrix();
			m_viewDirty = false;
		}
		return mView;
	}
	const glm::mat4& GetProjectionMatrix() const { return mProjection; }

	glm::vec3 GetPosition() const { return m_transform ? m_transform->GetPosition() : glm::vec3(0); }

	glm::vec3 GetRightVector() const
	{
		return m_transform ? m_transform->GetViewRight() : glm::vec3(1.f, 0.f, 0.f);
	}

	void SetAspect(float aspect)
	{
		m_aspect = aspect;
		UpdateProjectionMatrix();
	}

	float GetNear() const { return m_near; }
	float GetFar() const { return m_far; }
	float GetFov() const { return m_fov; }
	void SetFov(float fov)
	{
		m_fov = glm::clamp(fov, m_fovMin, m_fovMax);
		UpdateProjectionMatrix();
	}

private:
	bool MoveFreeFly(float deltaTime)
	{
		const glm::vec3 forward = m_transform->GetViewForward();
		const glm::vec3 right = m_transform->GetViewRight();
		const glm::vec3 worldUp(0.f, 1.f, 0.f);

		glm::vec3 delta = forward * m_axisForward + right * m_axisRight + worldUp * m_axisWorldUp;
		const float lenSq = glm::dot(delta, delta);
		if (lenSq < 1e-8f)
			return false;

		delta = glm::normalize(delta) * m_moveSpeed * (m_sprint ? m_sprintMultiplier : 1.f) * deltaTime;
		m_transform->AddPosition(delta);
		return true;
	}

	void UpdateViewMatrix()
	{
		if (!m_transform)
			return;
		const glm::vec3 pos = m_transform->GetPosition();
		const glm::vec3 forward = m_transform->GetViewForward();
		const glm::vec3 up = m_transform->GetViewUp();

		mView = glm::lookAt(pos, pos + forward, up);
	}

	void UpdateProjectionMatrix()
	{
		mProjection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
	}

	Transform* m_transform = nullptr;
	float m_fov = 60.f;
	float m_aspect = 16.f / 9.f;
	float m_near = 0.1f;
	float m_far = 1000.f;
	glm::mat4 mView = glm::mat4(1);
	glm::mat4 mProjection = glm::mat4(1);
	bool m_viewDirty = false;

	float m_pendingScrollDeltaY = 0.f;
	float m_fovMin = 10.f;
	float m_fovMax = 120.f;
	float m_fovDegreesPerScroll = 0.1f;

	double m_pendingMouseDx = 0.0;
	double m_pendingMouseDy = 0.0;
	float m_mouseSensitivity = 0.12f;
	float m_pitchMin = -89.f;
	float m_pitchMax = 89.f;

	float m_axisForward = 0.f;
	float m_axisRight = 0.f;
	float m_axisWorldUp = 0.f;
	float m_moveSpeed = 6.f;
	bool m_sprint = false;
	float m_sprintMultiplier = 2.5f;
};
