#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>

#include "../Core/Log.h"
#include "../Renderer/RenderContext.h"
#include "../Entity/Entity.h"
#include "../Entity/Components/Camera.h"
#include "Gui.h"



using RenderPipelineCallback = std::function<void(RenderContext&)>;

class Window
{
public:
	Window();
	~Window();
	bool Create(const int w, const int h, const char* title);
	void Run(RenderContext& context);

	int vWidth = 0;
	int vHeight = 0;
	float deltaTime = 0.f;
	RenderPipelineCallback renderCallback;

	void AccumulateScrollDeltaY(float dy) { m_scrollDeltaY += dy; }

	void switchCursorVisibility();

private:
	GLFWwindow* win = nullptr;
	float m_scrollDeltaY = 0.f;

	double m_mouseAccX = 0.0;
	double m_mouseAccY = 0.0;
	double m_lastMouseX = 0.0;
	double m_lastMouseY = 0.0;
	bool m_firstMouse = true;

	float m_axisForward = 0.f;
	float m_axisRight = 0.f;
	float m_axisWorldUp = 0.f;
	bool m_sprintHeld = false;

	bool m_showCursor = true;
	/// 上一帧 F1 是否处于按下（用于边沿检测，避免按住时每帧反复切换）
	bool m_f1KeyWasDown = false;
	Gui ui;
	friend void processInput(GLFWwindow* window);
	friend void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	friend void scroll_callback(GLFWwindow* window, double xoffse, double yoffset);
};
