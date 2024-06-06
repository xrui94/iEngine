#pragma once

#include "Scene.h"
#include "cameras/Camera.h"

// #include <glad/glad.h>
#include <GLFW/glfw3.h>

// #include <glm/glm.hpp>

#include <thread>
#include <atomic>


// void RenderLoopCallback(void* arg)
// {
//     static_cast<RenderThread*>(arg)->OnFrame();
// }

class RenderThread {
public:
    RenderThread(const std::unique_ptr<Window>& window);

    ~RenderThread();

public:
    bool IsRunning() const { return m_IsRunning; }

    void Start();

    void SetScene(const std::shared_ptr<Scene>& scene) { m_Scene = scene.get(); }

    // friend void RenderLoopCallback(void* arg); // 友元函数声明

private:
    void OnFrame();
    
    /// 渲染线程主函数
    void RenderMain();

private:
    Window* m_Window;

    Scene* m_Scene;

#if defined(IE_WGPU_NATIVE)
    std::thread m_RenderThread; //渲染线程
#elif defined(IE_ONLY_EMSCRIPTEN) || defined(IE_WGPU_EMSCRIPTEN)
    pthread_t m_RenderThread;   //渲染线程
#endif

    std::atomic<bool> m_IsRunning;
};
