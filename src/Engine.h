#pragma once

#include "windows/Window.h"
#include "RenderThread.h"
#include "Scene.h"
#include "Api.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex> 

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
    std::string backend = "webgpu";
    bool usingOffscreenCanvas = true;
    // bool customCanvas = false;
    std::string canvasId = "#xr-main-canvas";
    std::string style = "";
};


// class Scene;

class Engine
{
public:
    //
    static Engine& GetInstance(EngineInitOpts initOpts);

    std::unique_ptr<Window>& GetWindow() { return m_Window; }

    // friend void RenderLoopCallback(void* arg); // 友元函数声明

    void Init();

    void AddScene(std::shared_ptr<Scene> scene, bool active = true);

    void ActiveScene(const std::string& id = "");

    void AddMesh(std::shared_ptr<Mesh> mesh);

    void Run();

private:
    // 禁止外部构造
#if defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)
    Engine(EngineInitOpts initOpts);
#elif defined(IE_WGPU_NATIVE) || defined(IE_DAWN_NATIVE)
    Engine(EngineInitOpts initOpts);
#endif

    // 禁止外部析构
    ~Engine();

    // 禁止外部拷贝构造
    Engine(const Engine& engine) = delete;

    // 禁止外部赋值操作
    const Engine& operator=(const Engine& engine) = delete;

private:
    void OnFrame();

    void Render();

    // void Run();

    std::vector<std::shared_ptr<Mesh>> m_Meshes;

    std::mutex m_SceneMutex;  // 用于同步对m_Scene的访问

private:
    static Engine* s_Instance;

    EngineInitOpts m_InitOpts;

    bool m_IsRunning;

    std::unique_ptr<Window> m_Window;

    std::unique_ptr<RenderThread> m_RenderThread;

    std::shared_ptr<Scene> m_Scene;

    std::unordered_map<std::string, std::shared_ptr<Scene>> m_SceneMap;
};

