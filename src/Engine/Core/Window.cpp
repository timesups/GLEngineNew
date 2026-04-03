#include "Window.h"

#include "../Core/Log.h"
#include "GLFW/glfw3.h"

#include "../../../external/imgui/imgui.h"
#include "../../../external/imgui/imgui_internal.h"

#include <cmath>




static constexpr const char* kModule = "Window";

/// 将 Dock 中央节点映射到帧缓冲像素；失败则返回 false（调用方用整窗）
static bool SceneViewportFromDockSpace(ImGuiID dockspaceRootId, int fbW, int fbH, int* outX, int* outY, int* outW, int* outH)
{
	const ImGuiIO& io = ImGui::GetIO();
	if ((io.ConfigFlags & ImGuiConfigFlags_DockingEnable) == 0 || dockspaceRootId == 0 || fbW <= 0 || fbH <= 0)
		return false;

	ImGuiDockNode* central = ImGui::DockBuilderGetCentralNode(dockspaceRootId);
	if (!central || !central->IsVisible || central->Size.x < 1.f || central->Size.y < 1.f)
		return false;

	const ImGuiViewport* vp = ImGui::GetMainViewport();
	const float p0x = central->Pos.x - vp->Pos.x;
	const float p0y = central->Pos.y - vp->Pos.y;
	const float p1x = p0x + central->Size.x;
	const float p1y = p0y + central->Size.y;
	const float sx = (io.DisplaySize.x > 0.f) ? (static_cast<float>(fbW) / io.DisplaySize.x) : 1.f;
	const float sy = (io.DisplaySize.y > 0.f) ? (static_cast<float>(fbH) / io.DisplaySize.y) : 1.f;

	int x = static_cast<int>(std::floor(p0x * sx));
	int yTop = static_cast<int>(std::floor(p0y * sy));
	const int x1 = static_cast<int>(std::ceil(p1x * sx));
	const int y1 = static_cast<int>(std::ceil(p1y * sy));
	int rw = x1 - x;
	int rh = y1 - yTop;
	if (rw < 1 || rh < 1)
		return false;

	if (x < 0)
		x = 0;
	if (x > fbW - 1)
		x = fbW - 1;
	if (yTop < 0)
		yTop = 0;
	if (yTop > fbH - 1)
		yTop = fbH - 1;
	if (rw > fbW - x)
		rw = fbW - x;
	if (rh > fbH - yTop)
		rh = fbH - yTop;
	if (rw < 1 || rh < 1)
		return false;

	const int glY = fbH - yTop - rh;
	*outX = x;
	*outY = glY;
	*outW = rw;
	*outH = rh;
	return true;
}

static void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	Window* w = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!w)
		return;

	w->m_axisForward = 0.f;
	w->m_axisRight = 0.f;
	w->m_axisWorldUp = 0.f;

	// UI 模式：焦点在 ImGui 上时不处理行走键；游戏模式（隐藏光标）始终处理
	const bool gameOnly = !w->m_showCursor;
	const bool allowMoveKeys = gameOnly || !ImGui::GetIO().WantCaptureKeyboard;
	if (allowMoveKeys)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			w->m_axisForward += 1.f;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			w->m_axisForward -= 1.f;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			w->m_axisRight += 1.f;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			w->m_axisRight -= 1.f;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			w->m_axisWorldUp += 1.f;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			w->m_axisWorldUp -= 1.f;
	}
	const int f1State = glfwGetKey(window, GLFW_KEY_F1);
	const bool f1Down = (f1State == GLFW_PRESS);
	if (f1Down && !w->m_f1KeyWasDown)
		w->switchCursorVisibility();
	w->m_f1KeyWasDown = f1Down;

	if (allowMoveKeys)
		w->m_sprintHeld = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	else
		w->m_sprintHeld = false;
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Window* w = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!w)
		return;
	// 显示光标 / UI 模式：鼠标交给 ImGui，不累计视角
	if (w->m_showCursor)
		return;
	if (w->m_firstMouse)
	{
		w->m_lastMouseX = xpos;
		w->m_lastMouseY = ypos;
		w->m_firstMouse = false;
		return;
	}
	w->m_mouseAccX += xpos - w->m_lastMouseX;
	w->m_mouseAccY += ypos - w->m_lastMouseY;
	w->m_lastMouseX = xpos;
	w->m_lastMouseY = ypos;
}

static void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset)
{
	Window* w = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (w && !w->m_showCursor)
		w->AccumulateScrollDeltaY(static_cast<float>(yoffset));
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (myWindow)
	{
		myWindow->vWidth = width;
		myWindow->vHeight = height;
	}
	if (width <= 0 || height <= 0)
		Log(kModule, LogLevel::WARNING, "Framebuffer size invalid: {}x{}", width, height);
	glViewport(0, 0, width, height);
}

Window::Window()
{
	if (!glfwInit())
		Log(kModule, LogLevel::ERROR, "glfwInit failed");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

Window::~Window()
{

	ui.Clean();
	//清理资源
	if (win)
		glfwDestroyWindow(win);
	glfwTerminate();
}

void Window::switchCursorVisibility()
{
	m_showCursor = !m_showCursor;
	m_firstMouse = true;
	if (win)
		glfwSetInputMode(win, GLFW_CURSOR, m_showCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

bool Window::Create(const int w, const int h, const char* title)
{
	vWidth = w;
	vHeight = h;
	win = glfwCreateWindow(vWidth, vHeight, title, NULL, NULL);
	if (!win)
	{
		Log(kModule, LogLevel::ERROR, "Failed to create window.");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(win);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Log(kModule, LogLevel::ERROR, "Failed to load glad");
		return false;
	}

	GLint glMajor = 0, glMinor = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
	glGetIntegerv(GL_MINOR_VERSION, &glMinor);

	glViewport(0, 0, vWidth, vHeight);

	glfwSetWindowUserPointer(win, this);
	glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
	glfwSetCursorPosCallback(win, mouse_callback);
	glfwSetScrollCallback(win, scroll_callback);

	m_firstMouse = true;
	glfwSetInputMode(win, GLFW_CURSOR, m_showCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

	Log(kModule, LogLevel::INFO, "Window created {}x{}, OpenGL {}.{}", vWidth, vHeight, glMajor, glMinor);
	//初始化imgui
	ui.Init(win);
	return true;
}

void Window::Run(RenderContext& context)
{
	float lastTime = static_cast<float>(glfwGetTime());
	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		const float now = static_cast<float>(glfwGetTime());
		deltaTime = now - lastTime;
		lastTime = now;

		context.deltaTime = deltaTime;
		context.framebufferWidth = vWidth;
		context.framebufferHeight = vHeight;

		// F1：m_showCursor true = UI 模式（ImGui 吃输入）；false = 游戏模式（GLFW/相机）
		const bool gameInputExclusive = !m_showCursor;
		ui.BeginFrame(gameInputExclusive);

		processInput(win);

		if (context.currentCamera)
		{
			if (Camera* cam = context.currentCamera->GetComponent<Camera>())
			{
				cam->QueueScrollDeltaY(m_scrollDeltaY);
				cam->QueueMouseLookDelta(m_mouseAccX, m_mouseAccY);
				cam->SetMoveAxes(m_axisForward, m_axisRight, m_axisWorldUp);
				cam->SetSprint(m_sprintHeld);
			}
		}
		m_scrollDeltaY = 0.f;
		m_mouseAccX = 0.0;
		m_mouseAccY = 0.0;

		const ImGuiID dockId = ui.BuildWidgets();

		int svx = 0;
		int svy = 0;
		int svw = vWidth;
		int svh = vHeight;
		if (!SceneViewportFromDockSpace(dockId, vWidth, vHeight, &svx, &svy, &svw, &svh))
		{
			svx = 0;
			svy = 0;
			svw = vWidth;
			svh = vHeight;
		}
		context.sceneViewportX = svx;
		context.sceneViewportY = svy;
		context.sceneViewportWidth = svw;
		context.sceneViewportHeight = svh;
		context.width = svw;
		context.height = svh;

		if (context.currentCamera)
		{
			if (Camera* cam = context.currentCamera->GetComponent<Camera>())
			{
				const float aspect = (svh > 0) ? (static_cast<float>(svw) / static_cast<float>(svh)) : (16.f / 9.f);
				cam->SetAspect(aspect);
			}
		}

		if (renderCallback)
			renderCallback(context);

		ui.EndFrame();

		glfwSwapBuffers(win);
	}
	Log(kModule, LogLevel::INFO, "Main loop exited");
}

