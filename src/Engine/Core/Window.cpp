#include "Window.h"

#include "../Core/Log.h"

static constexpr const char* kModule = "Window";

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

	w->m_sprintHeld = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Window* w = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!w)
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
	if (w)
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
	glViewport(0, 0, width, height);
}

Window::Window()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

Window::~Window()
{
	//清理资源
	if (win)
		glfwDestroyWindow(win);
	glfwTerminate();
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

	glViewport(0, 0, vWidth, vHeight);

	glfwSetWindowUserPointer(win, this);
	glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
	glfwSetCursorPosCallback(win, mouse_callback);
	glfwSetScrollCallback(win, scroll_callback);

	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_firstMouse = true;

	return true;
}

void Window::Run(RenderContext& context)
{
	float lastTime = static_cast<float>(glfwGetTime());
	while (!glfwWindowShouldClose(win))
	{
		const float now = static_cast<float>(glfwGetTime());
		deltaTime = now - lastTime;
		lastTime = now;

		context.deltaTime = deltaTime;
		context.width = vWidth;
		context.height = vHeight;

		processInput(win);

		if (context.currentCamera)
		{
			if (Camera* cam = context.currentCamera->GetComponent<Camera>())
			{
				cam->QueueScrollDeltaY(m_scrollDeltaY);
				cam->QueueMouseLookDelta(m_mouseAccX, m_mouseAccY);
				cam->SetMoveAxes(m_axisForward, m_axisRight, m_axisWorldUp);
				cam->SetSprint(m_sprintHeld);
				const float aspect = (vHeight > 0) ? (static_cast<float>(vWidth) / static_cast<float>(vHeight)) : (16.f / 9.f);
				cam->SetAspect(aspect);
			}
		}
		m_scrollDeltaY = 0.f;
		m_mouseAccX = 0.0;
		m_mouseAccY = 0.0;

		if (renderCallback)
			renderCallback(context);

		glfwSwapBuffers(win);
		glfwPollEvents();
	}
}

