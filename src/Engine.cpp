#include "Engine.h"

#ifdef IE_GLFW_WINDOW
    #include "windows/GlfwWindow.h"
#elif defined(IE_HTML_WINDOW)
    #include "windows/HtmlWindow.h"
#endif

#include <iostream>


// 静态成员变量在类中仅仅是声明，没有定义，
// 所以要在类的外面定义，实际上是给静态成员变量分配内存
// 否则，将会在程序链接的时候，报“error LNK2019”的错误
Engine* Engine::s_Instance = nullptr;

#if defined(IE_WGPU_NATIVE)
    Engine::Engine(const std::string& name) : m_IsRunning(true)
    {
        //
        s_Instance = this;

        //
        m_Window = Window::Create(WindowProps(name));
    }
#elif defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)
    Engine::Engine(EngineInitOpts initOpts) : m_IsRunning(true)
    {
        //
        s_Instance = this;

        //
        // m_Window = Window::Create(WindowProps(name, canvasId));

        WindowProps winProps(initOpts.containerId);
        // winProps.ContainerId = initOpts.containerId;
        winProps.Width = initOpts.width;
        winProps.Height = initOpts.height;
        winProps.CanvasId = initOpts.canvasId;
        winProps.Style = initOpts.style;
        m_Window = Window::Create(winProps);

    }
#endif

Engine::~Engine()
{
}

void Engine::Run()
{
#ifdef IE_GLFW_WINDOW
    GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
    while (!glfwWindowShouldClose(glfwWindow))
    {
        //非渲染相关的API，例如处理系统事件，就放到主线程中。
        glfwPollEvents();
    }

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
#elif defined(IE_HTML_WINDOW)
    HtmlWindow* htmlwindow = static_cast<HtmlWindow*>(m_Window->GetNativeWindow());
    // while (!htmlwindow->ShouleClose())
    // {

    // }
            
    // EM_ASM(noExitRuntime=true);

    emscripten_set_main_loop([](){
        // std::cout << "Rendering..." << std::endl;
    }, 0, false);
#endif
}

void Engine::AddScene(std::shared_ptr<Scene> scene, bool active)
{
    std::string id = scene->GetId();
    auto iter = m_SceneMap.find(id);
    if (iter == m_SceneMap.end()) m_SceneMap[id] = scene;

    if (active)
    {
    #ifdef IE_GLFW_WINDOW
        GlfwWindow& glfwWindow = static_cast<GlfwWindow&>(m_Window->Get());
        glfwWindow.RegisterEvent();
        glfwWindow.SetScene(m_SceneMap[id]);
    #elif defined(IE_HTML_WINDOW)
        HtmlWindow& htmlWindow = static_cast<HtmlWindow&>(m_Window->Get());
        htmlWindow.RegisterEvent();
        htmlWindow.SetScene(m_SceneMap[id]);
    #endif

        // 创建渲染线程
        m_RenderThread = std::make_unique<RenderThread>(m_Window);
        m_RenderThread->Start();
        m_RenderThread->SetScene(m_SceneMap[id]);
    }

    //return m_SceneMap[id];

    //
    Run();

// #if defined(IE_HTML_WINDOW)
//     EM_ASM(noExitRuntime=true);
// #endif
}

void Engine::ActiveScene(const std::string& id)
{
    auto iter = m_SceneMap.find(id);
    if (iter != m_SceneMap.end())
    {
    #ifdef IE_GLFW_WINDOW
        GlfwWindow& glfwWindow = static_cast<GlfwWindow&>(m_Window->Get());
        glfwWindow.RegisterEvent();
        glfwWindow.SetScene(iter->second);
    #elif defined(IE_HTML_WINDOW)
        HtmlWindow& htmlWindow = static_cast<HtmlWindow&>(m_Window->Get());
        htmlWindow.RegisterEvent();
        htmlWindow.SetScene(m_SceneMap[id]);
    #endif
        // 如果渲染线程正在运行，则先停止渲染线程
        //if (m_RenderThread->IsRunning()) m_RenderThread->Stop();

        // 创建渲染线程
        m_RenderThread = std::make_unique<RenderThread>(m_Window);
        m_RenderThread->SetScene(iter->second);

        //
        Run();
    }
}