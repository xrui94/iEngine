#pragma once

#include "Mesh.h"
#include "webgpu/WebGPUDevice.h"

#include <glm/vec4.hpp>

#include <memory>

class Renderer
{
public:
    Renderer();

    ~Renderer() = default;
    
public:
    void Resize(uint32_t width, uint32_t height);

    // void Render(std::shared_ptr<Mesh> mesh, glm::vec4 clearColor = glm::vec4(49.f, 77.f, 121.f, 1.0f));

    std::optional<Frame> BeginFrame(glm::vec4 clearColor = glm::vec4(49.f, 77.f, 121.f, 1.0f));

    void Render(Frame& frame, std::shared_ptr<Mesh> mesh);

    void EndFrame(Frame& frame);

private:
    uint32_t m_Width;
    uint32_t m_Height;
};