#pragma once

#include "Camera.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct GlobalUniforms {
    // We add transform matrices
    glm::mat4x4 projectionMatrix = glm::mat4(1.0f);
    glm::mat4x4 viewMatrix = glm::mat4(1.0f);
    glm::vec3 cameraWorldPosition = glm::vec3(0.0f);
    float time = 0.f;
    float gamma = 0.f;
    float _pad1[3] = { 0.f };
};

class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera(float aspect);

    ~PerspectiveCamera();

public:
    void Destroy() override;

    void UpdateProjectionMatrix(float aspect) override;

    void UpdateViewMatrix() override;

    void UpdateDragInertia();

private:
    void Init() override;

private:
    glm::vec3 m_Position;

    glm::vec3 m_Up;

    float m_Fov;

    float m_Aspect;

    float m_Near;

    float m_Far;

    GlobalUniforms m_GlobalUniforms;

    wgpu::Buffer m_CameraUniformBuffer;
};