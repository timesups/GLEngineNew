#pragma once
#include "GLFW/glfw3.h"

#include "../../../external/imgui/imgui.h"
#include "../../../external/imgui/backends/imgui_impl_glfw.h"
#include "../../../external/imgui/backends/imgui_impl_opengl3.h"


class Gui
{
public:
	Gui() = default;
	~Gui() = default;
	void Init(GLFWwindow* win)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;

		// 须先注册本窗口的 GLFW 回调，再 Init(..., true)，ImGui 会链接到你已有的回调之后
		ImGui_ImplGlfw_InitForOpenGL(win, true);
		ImGui_ImplOpenGL3_Init("#version 420 core");
	}

	/// @param gameInputExclusive true = 当前帧输入交给游戏（隐藏光标模式），ImGui 不消费键鼠；false = UI 模式，ImGui 可交互
	void BeginFrame(bool gameInputExclusive)
	{
		ImGuiIO& io = ImGui::GetIO();
		const ImGuiConfigFlags mask = ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard | ImGuiConfigFlags_NoMouseCursorChange;
		if (gameInputExclusive)
			io.ConfigFlags |= mask;
		else
			io.ConfigFlags &= ~mask;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	/// @return DockSpace 根节点 id（与 DockSpaceOverViewport 一致），供主循环查询中央游戏区矩形
	ImGuiID BuildWidgets()
	{
		// 主视口全屏 Dock；中央节点穿透，底下由 Renderer 先画的 3D 可从中间露出来，面板拖到四周停靠
		const ImGuiID dockId = ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::ShowDemoWindow();
		return dockId;
	}

	void EndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Clean()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
};