#include "Scene.h"

#include "renderers/webgpu/WebGPUDevice.h"

#include <iostream>

Scene::Scene(uint32_t width, uint32_t height, std::shared_ptr<Camera> camera)
    : m_Id("0x000001"), m_IsOutdated(true), m_Width(width), m_Height(height),
    m_Camera(camera)
{
    m_Renderer = std::make_unique<Renderer>();
    //m_Renderer->Resize(width, height);
}

Scene::~Scene()
{
    Clear();
}

void Scene::AddMesh(std::shared_ptr<Mesh> mesh)
{
    m_Meshes.push_back(mesh);
}

void Scene::AddCamera(std::shared_ptr<Camera> camera)
{
    m_Camera = camera;
}

void Scene::DestroyCamera()
{
    // m_Camera->Destroy();
}

void Scene::Render()
{
    if (m_Camera == nullptr)
    {
        std::cerr << "Error: An valid \"Perspective Camera\" or \"Orthographic Camera\" is required." << std::endl;
        return;
    }
    if (!m_Camera->IsInitialized()) m_Camera->Init();

    auto frame = m_Renderer->BeginFrame(glm::vec4(49.f / 255, 77.f / 255, 121.f / 255, 1.0f));
    if (!frame.has_value()) return;

    for (auto& mesh : m_Meshes)
    {
        if (m_IsOutdated)
        {
            std::vector<ResourceBindGroup> resourceBindGroup = {
                {
                    "CameraUniform",
                    m_Camera->GetBindGroupLayout(),
                    m_Camera->GetBindGroup()
                } 
            };
            mesh->UpdateBindGroup(resourceBindGroup);
        }

        if (!mesh->IsPrepared())
        {
            mesh->SendToBuffer();
        }

        m_Renderer->Render(frame.value(), mesh);
    }

    //
    m_IsOutdated = false;

    m_Renderer->EndFrame(frame.value());
}

void Scene::RenderOnce()
{

}

void Scene::RenderLoop()
{

}

void Scene::Clear()
{
    m_Camera->Destroy();

    for (auto& mesh : m_Meshes)
    {
        if (!mesh->IsPrepared())
        {
            mesh->Destroy();
        }
    }

    m_Meshes.clear();
}

void Scene::Resize(uint32_t width, uint32_t height)
{
    // 当窗口宽和高为0时，则不再响应resize事件，否则，创建SwapChain时，程序将崩溃
    if (width == 0 || height == 0) return;

    m_Renderer->Resize(width, height);
}

void Scene::Rotate()
{

}

void Scene::Translate()
{

}

void Scene::Zoom()
{

}

void Scene::Pickup()
{

}