#pragma once
#include <memory>
#include <vector>


#include "../Entity/Entity.hpp"
#include "../Entity/Components/Camera.hpp"


class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;
    // 设置主相机
    void SetCamera(std::shared_ptr<Camera> camera)
    {
        m_mainCamera = camera;
    }

    // 渲染场景
    void Render()
    {

    }

private:
    std::shared_ptr<Camera> m_mainCamera;
};