#pragma once

#include "Window.h"
#include "Scene.h"
#include "../cameras/Camera.h"

#ifdef IE_WGPU_NATIVE
    #include "glfw/glfw3.h"
#endif

#include <mutex>
#include <condition_variable>

class GlfwWindow : public Window
{
public:
    GlfwWindow(const WindowProps& props);

public:
    virtual void* GetNativeWindow() const override { return m_Window; }

    virtual uint32_t GetWidth() const  override{ return m_Width; }

    virtual uint32_t GetHeight() const  override{ return m_Height; }

    void SetScene(const std::shared_ptr<Scene>& scene) { m_Scene = scene.get(); }

    void RegisterEvent();

    virtual void OnResize(uint32_t width, uint32_t height) override;

    virtual void OnMouseButton(int32_t button, int32_t action, int32_t/* mods*/) override;

    virtual void OnMouseMove(double xPos, double yPos) override;

    virtual void OnScroll(double xOffset, double yOffset) override;

    virtual bool GetSwapchainState() override;

    virtual void SetSwapchainState(bool updated) override;

private:
    GLFWwindow* m_Window;

    int32_t m_Width;

    int32_t m_Height;

    std::string m_Title;
    
    Scene* m_Scene;

    DragState m_Drag;

    bool m_SwapchainUpdated;    //

    std::mutex m_Mutex;

    std::condition_variable m_ConditionVar;
};