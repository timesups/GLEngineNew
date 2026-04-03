#include "Renderer.h"

#include "../Core/Log.h"
#include "../Entity/Components/Camera.h"
#include "Asset/Mesh.h"

static constexpr const char* kModule = "Renderer";

Renderer::Renderer()
{
	cameraBuffer.BindBase(0);
	Log(kModule, LogLevel::INFO, "Uniform buffer bound to binding 0 (camera_buffer)");

	//启用各种测试
	glEnable(GL_DEPTH_TEST);
}

void Renderer::Render(RenderContext& context)
{
	//更新着色器
	LoaderManager::Get().UpdateAssetFromDisk();

	const int vx = context.sceneViewportX;
	const int vy = context.sceneViewportY;
	const int vw = context.sceneViewportWidth > 0 ? context.sceneViewportWidth : context.width;
	const int vh = context.sceneViewportHeight > 0 ? context.sceneViewportHeight : context.height;
	const int fbw = context.framebufferWidth > 0 ? context.framebufferWidth : vw;
	const int fbh = context.framebufferHeight > 0 ? context.framebufferHeight : vh;

	// 先整帧缓冲清屏，避免 Dock 旁区域保留旧像素；再只在中央区做 3D 投影
	glViewport(0, 0, fbw, fbh);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glViewport(vx, vy, vw, vh);
	//相机更新
	static bool s_loggedNoCameraEntity = false;
	static bool s_loggedNoCameraComponent = false;
	if (!context.currentCamera)
	{
		if (!s_loggedNoCameraEntity)
		{
			Log(kModule, LogLevel::WARNING, "Render skipped: no currentCamera in RenderContext");
			s_loggedNoCameraEntity = true;
		}
		return;
	}
	Camera* cam = context.currentCamera->GetComponent<Camera>();
	if (!cam)
	{
		if (!s_loggedNoCameraComponent)
		{
			Log(kModule, LogLevel::ERROR, "currentCamera entity has no Camera component");
			s_loggedNoCameraComponent = true;
		}
		return;
	}
	cam->SetAspect((float)context.width / (context.height > 0 ? context.height : 1));
	
	//Uniform buffer传递
	DataInputsStd140 data{};
	data.GL_MATRIX_P = cam->GetProjectionMatrix();
	data.GL_MATRIX_V = cam->GetViewMatrix();
	data._CameraPosition_zNear = glm::vec4(cam->GetPosition(), cam->GetNear());
	data._zFar_pad.x = cam->GetFar();
	cameraBuffer.UploadStruct(data);


	//更新实体
	EntityManager::Get().Update(context.deltaTime);
	//绘制所有实体
	EntityManager::Get().Render();
}

