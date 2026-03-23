#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>//负责创建opengl的窗口

#include "../Engine/core/Log.hpp"


#define MODULE "Window"



void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	//	cam.Move(deltaTime, MOVEDIRECTION::FORWARD);
	//if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	//	cam.Move(deltaTime, MOVEDIRECTION::BACKWARD);
	//if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	//	cam.Move(deltaTime, MOVEDIRECTION::RIGHT);
	//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	//	cam.Move(deltaTime, MOVEDIRECTION::LEFT);
	//if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	//	cam.Move(deltaTime, MOVEDIRECTION::UP);
	//if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	//	cam.Move(deltaTime, MOVEDIRECTION::DOWN);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//if (firstMouse)
	//{
	//	lastX = xpos;
	//	lastY = ypos;
	//	firstMouse = false;
	//}

	//float xoffset = xpos - lastX;
	//float yoffset = lastY - ypos;
	//lastX = xpos;
	//lastY = ypos;

	//cam.RotateCamera(deltaTime, xoffset, yoffset);

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void scroll_callback(GLFWwindow* window, double xoffse, double yoffset)
{
	//cam.SetFov(cam.GetFov() + yoffset * 0.1f);
}

class Window 
{
public:
	Window()
	{
		//设置glfw基本属性
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}
	~Window() 
	{
		glfwTerminate();
		glfwDestroyWindow(win);
	}
	bool Create(const int w, const int h,const char* title)
	{
		vWidth = w;
		vHeight = h;
		win = glfwCreateWindow(vWidth, vHeight, title, NULL, NULL);
		if(!win)
		{

			Log(MODULE, LogLevel::ERROR, "Failed to create window.");
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(win);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
		{
			Log(MODULE, LogLevel::ERROR, "Failed to load glad");
			return false;
		}

		glViewport(0, 0, vWidth, vHeight);

		//设置用户指针
		glfwSetWindowUserPointer(win, this);
		//一些回调函数
		glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
		glfwSetCursorPosCallback(win, mouse_callback);
		glfwSetScrollCallback(win, scroll_callback);


		glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//隐藏鼠标显示

	}
	bool IsRunning() 
	{
		bool running = !glfwWindowShouldClose(win);
		if(running)
		{
			processInput(win);
		}

		return running;
	}
	void Update()
	{
		glfwSwapBuffers(win);
		glfwPollEvents();
	}
public:
	int vWidth;
	int vHeight;
private:
	GLFWwindow* win = nullptr;

};



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (myWindow) {
		myWindow->vWidth = width;
		myWindow->vHeight = height;
	}
	glViewport(0, 0, width, height);
}
