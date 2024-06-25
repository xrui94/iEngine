#pragma once

#include "Scene.h"
#include "cameras/Camera.h"

// #include <glad/glad.h>
#include <GLFW/glfw3.h>

// #include <glm/glm.hpp>

#include <thread>
#include <atomic>


class RenderThread {
public:
    RenderThread(const std::unique_ptr<Window>& window);

    ~RenderThread();

public:
    bool IsRunning() const { return m_IsRunning; }

    void Start(void*(*fn)(void*), void* userData = nullptr);

private:
    Window* m_Window;

    //渲染线程
#if defined(IE_WGPU_NATIVE) || defined(IE_DAWN_NATIVE)
    std::thread m_RenderThread;
#elif defined(IE_ONLY_EMSCRIPTEN) || defined(IE_WGPU_EMSCRIPTEN)
    pthread_t m_RenderThread;
#endif

    std::atomic<bool> m_IsRunning;
};
