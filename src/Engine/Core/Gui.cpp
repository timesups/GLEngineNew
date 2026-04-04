#include "Gui.h"

#include <cstdio>

#include "Log.h"
#include "../Entity/EntityManager.h"
#include "../Entity/Components/Camera.h"
#include "../Entity/Components/MeshRender.h"
#include "../Entity/Components/Transform.h"



void Gui::Init(GLFWwindow* win)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;

    // Install after window callbacks; ImGui chains into existing GLFW callbacks.
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 420 core");
}

void Gui::BeginFrame(bool gameInputExclusive)
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


ImGuiID Gui::BuildWidgets()
{
    // Full-viewport dock; central node passthrough for 3D scene underneath.
    const ImGuiID dockId = ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    ShowMenu();
    ShowLog();
    ShowDetail();
    ShowOutline();

    return dockId;
}


void Gui::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::Clean()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
void Gui::ShowMenu()
{
}
void Gui::ShowLog()
{
    std::string logOut;
    while (LogManager::Get().GetALog(logOut))
    {
        logWindow.AppendLog(logOut);
    }
    logWindow.Draw("Log");
}
void Gui::ShowDetail()
{
    if (!ImGui::Begin("detail"))
    {
        ImGui::End();
        return;
    }

    const auto& entities = EntityManager::Get().GetEntities();
    const size_t count = entities.size();
    if (count == 0)
    {
        ImGui::TextUnformatted("No entities in scene.");
        ImGui::End();
        return;
    }

    int idx = item_selected_idx;
    if (idx < 0 || static_cast<size_t>(idx) >= count)
        idx = static_cast<int>(count - 1);
    item_selected_idx = idx;

    const std::shared_ptr<Entity>& ent = entities[static_cast<size_t>(idx)];
    if (!ent)
    {
        ImGui::TextUnformatted("Invalid entity pointer.");
        ImGui::End();
        return;
    }

    if (m_detailLastSelectedIdx != item_selected_idx)
    {
        m_detailLastSelectedIdx = item_selected_idx;
        std::snprintf(m_detailNameBuf, sizeof(m_detailNameBuf), "%s", ent->m_name.c_str());
    }

    ImGui::InputText("Name", m_detailNameBuf, IM_ARRAYSIZE(m_detailNameBuf));
    ent->m_name = m_detailNameBuf;

    ImGui::Separator();
    ImGui::TextUnformatted("Components");
    ImGui::BulletText("Transform: %s", ent->HasComponent<Transform>() ? "yes" : "no");
    ImGui::BulletText("Camera: %s", ent->HasComponent<Camera>() ? "yes" : "no");
    ImGui::BulletText("MeshRender: %s", ent->HasComponent<MeshRender>() ? "yes" : "no");

    if (Transform* tr = ent->GetComponent<Transform>())
    {
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            glm::vec3 pos = tr->GetPosition();
            if (ImGui::DragFloat3("Position", &pos.x, 0.05f))
                tr->SetPosition(pos);
            glm::vec3 rot = tr->GetRotation();
            if (ImGui::DragFloat3("Rotation (deg)", &rot.x, 0.5f))
                tr->SetRotation(rot);
            glm::vec3 scale = tr->GetScale();
            if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f, 1000.f))
                tr->SetScale(scale);
        }
    }

    if (Camera* cam = ent->GetComponent<Camera>())
    {
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            float fov = cam->GetFov();
            if (ImGui::SliderFloat("FOV", &fov, 10.f, 120.f))
                cam->SetFov(fov);
            ImGui::Text("Clip near %.2f  far %.2f", cam->GetNear(), cam->GetFar());
        }
    }

    if (MeshRender* mr = ent->GetComponent<MeshRender>())
    {
        if (ImGui::CollapsingHeader("MeshRender"))
        {
            const std::shared_ptr<Model>& model = mr->GetModel();
            if (model)
            {
                ImGui::Text("Model: %s", model->m_name.c_str());
                ImGui::TextDisabled("Path: %s", model->m_path.c_str());
            }
            else
                ImGui::TextUnformatted("No model bound.");
        }
    }

    ImGui::End();
}
void Gui::ShowOutline()
{
    if (!ImGui::Begin("outline"))
    {
        ImGui::End();
        return;
    }

    const int entityCount = static_cast<int>(EntityManager::Get().GetEntityCount());
    if (entityCount <= 0)
        item_selected_idx = 0;
    else if (item_selected_idx < 0 || item_selected_idx >= entityCount)
        item_selected_idx = entityCount - 1;

    if (ImGui::BeginListBox(" ", ImVec2(-FLT_MIN, -FLT_MIN)))
    {
        for (int n = 0; n < entityCount; n++)
        {
            const bool is_selected = (item_selected_idx == n);
            if (ImGui::Selectable(EntityManager::Get().GetEntities()[n]->m_name.c_str(), is_selected))
                item_selected_idx = n;

            //if (item_highlight && ImGui::IsItemHovered())
            //    item_highlighted_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }


    ImGui::End();
}
