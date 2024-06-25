#include "Renderer.h"

// #include <glad/glad.h>

Renderer::Renderer()
    : m_Width(0), m_Height(0)
{

}

void Renderer::Resize(uint32_t width, uint32_t height)
{
    // ratio = width / (float) height;

    // glViewport(0, 0, width, height);
    // m_Width = width;
    // m_Height = height;

    // 重新创建Surface，`SurfaceOutput` must be dropped before a new `Surface` is made
    // WebGPUDevice::Get().CreareSurface();

    // 先销毁swap chain
    WebGPUDevice::Get().DestroySwapChain();

    // 重新创建swap chain
    WebGPUDevice::Get().CreateSwapChain(width, height);
}

std::optional<Frame> Renderer::BeginFrame(glm::vec4 clearColor)
{
    return WebGPUDevice::Get().Begin(clearColor);
}

void Renderer::Render(Frame& frame, std::shared_ptr<Mesh> mesh)
{
    WebGPUDevice::Get().Draw(frame, mesh);
}

void Renderer::EndFrame(Frame& frame)
{
    WebGPUDevice::Get().End(frame);
}