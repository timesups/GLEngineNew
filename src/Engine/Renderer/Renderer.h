#pragma once
#include <glad/glad.h>
#include <memory>
#include "RenderContext.h"

#include "../Entity/EntityManager.h"
#include "../Core/LoaderManager.h"
#include "UniformBuffer.h"


class Renderer
{
public:
    Renderer();
    ~Renderer() = default;

    // 渲染场景
    void Render(RenderContext& context);
private:
    UniformBuffer cameraBuffer{ sizeof(DataInputsStd140) };
};