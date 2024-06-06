#pragma once

#include "windows/Window.h"
#include "RenderThread.h"

#include <string>
#include <unordered_map>
#include <memory>

struct EngineInitOpts
{
    // EngineInitOpts(std::string containerId,uint32_t width, uint32_t height) 
    //     : containerId(containerId), width(width), height(height),
    //     backend("webgpu"), usingOffscreenCanvas(true), 
    //     customCanvas(false), canvasId("#canvas"), style("")
    // {
    // }

    std::string containerId;
    uint32_t width;
    uint32_t height;
    // std::string backend = "webgpu";
    // bool usingOffscreenCanvas = true;
    // bool customCanvas = false;
    std::string canvasId = "#xr-main-canvas";
    std::string style = "";
};

class Scene;

class Engine
{
public:

#if defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)
    Engine(EngineInitOpts initOpts);
#elif defined(IE_WGPU_NATIVE)
    Engine(const std::string& name);
#endif

    ~Engine();

    static Engine& Get() { return *s_Instance; }

    std::unique_ptr<Window>& GetWindow() { return m_Window; }

    void AddScene(std::shared_ptr<Scene> scene, bool active = true);

    void ActiveScene(const std::string& id);

private:
    void Run();

private:
    static Engine* s_Instance;

    bool m_IsRunning;

    std::unique_ptr<Window> m_Window;

    std::unique_ptr<RenderThread> m_RenderThread;

    std::unordered_map<std::string, std::shared_ptr<Scene>> m_SceneMap;
};

