#pragma once
#include <glad/glad.h>
#include <memory>
#include "RenderContext.hpp"

#include "../Entity/EntityManager.hpp"
#include "../Core/LoaderManager.hpp"
#include "UniformBuffer.hpp"


class Renderer
{
public:
    Renderer()
    {
        cameraBuffer.BindBase(0);
    }
    ~Renderer() = default;

    // 渲染场景
    void Render(RenderContext& context)
    {
        //更新着色器
        LoaderManager::Get().UpdateAssetFromDisk();


        glEnable(GL_DEPTH_TEST);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        EntityManager::Get().Update(context.deltaTime);
        //传递数据
        auto cam = context.currentCamera->GetComponent<Camera>();
        cam->SetAspect((float)context.width / context.height);
        DataInputsStd140 data{};
        data.GL_MATRIX_P = cam->GetProjectionMatrix();
        data.GL_MATRIX_V = cam->GetViewMatrix();
        data._CameraPosition_zNear = glm::vec4(cam->GetPosition(), cam->GetNear());
        data._zFar_pad.x = cam->GetFar();
        cameraBuffer.UploadStruct(data);


        EntityManager::Get().Render();

    }
private:
    UniformBuffer cameraBuffer{ sizeof(DataInputsStd140) };
};