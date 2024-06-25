#pragma once

#include <glm/vec2.hpp>

#if defined(IE_WGPU_NATIVE) || defined(IE_WGPU_EMSCRIPTEN)
    #include <webgpu/webgpu.hpp>
#elif defined(IE_DAWN_NATIVE) || defined(IE_ONLY_EMSCRIPTEN)
    #include <webgpu/webgpu_cpp.h>
#endif

struct CameraState {
    // angles.x is the rotation of the camera around the global vertical axis, affected by mouse.x
    // angles.y is the rotation of the camera around its local horizontal axis, affected by mouse.y
    glm::vec2 angles = { 0.8f, 0.5f };
    // zoom is the position of the camera along its local forward axis, affected by the scroll wheel
    float zoom = -1.2f;
};

struct DragState {
    // Whether a drag action is ongoing (i.e., we are between mouse press and mouse release)
    bool active = false;
    // The position of the mouse at the beginning of the drag action
    glm::vec2 startMouse = glm::vec2{0.0f};

    // The camera state at the beginning of the drag action
    CameraState startCameraState{};

    // Constant settings
    float sensitivity = 0.01f;
    float scrollSensitivity = 0.1f;

    // Inertia
    glm::vec2 velocity = { 0.0, 0.0 };
    glm::vec2 previousDelta = { 0.0, 0.0 };
    float intertia = 0.9f;
};

class Camera
{
public:
    Camera();

    virtual ~Camera();  // 加上“virtual”关键字，否则报警告：“warning: delete called on 'Camera' that is abstract but has non-virtual destructor”

public:
    virtual void Destroy() = 0;

    bool IsInitialized() const { return m_IsInitialized; }

    float GetXAngle() const { return m_XAngle; }

    float GetYAngle() const { return m_YAngle; }

    void SetXAngle(float xAngle) { m_XAngle = xAngle; }

    void SetYAngle(float yAngle) { m_YAngle = yAngle; }

    float GetZoom() const { return m_Zoom; }

    void SetZoom(float zoom) { m_Zoom = zoom; }

    wgpu::BindGroupLayout GetBindGroupLayout() const { return m_BindGroupLayout; }

    wgpu::BindGroup GetBindGroup() const { return m_BindGroup; }

    virtual void UpdateViewMatrix() = 0;

    virtual void UpdateProjectionMatrix(float aspect) = 0;

protected:
    virtual void Init() = 0;

    friend class Scene;

protected:
    bool m_IsInitialized;

    // angles.x is the rotation of the camera around the global vertical axis, affected by mouse.x
    float m_XAngle;

    // angles.y is the rotation of the camera around its local horizontal axis, affected by mouse.y
    float m_YAngle;

    // zoom is the position of the camera along its local forward axis, affected by the scroll wheel
    float m_Zoom;

    wgpu::BindGroupLayout m_BindGroupLayout;

    wgpu::BindGroup m_BindGroup;
};