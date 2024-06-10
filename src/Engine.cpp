#include "Engine.h"

#ifdef IE_GLFW_WINDOW
    #include "windows/GlfwWindow.h"
#elif defined(IE_HTML_WINDOW)
    #include "windows/HtmlWindow.h"
#endif

#include <iostream>


// void RenderLoopCallback(void* arg)
// {
//     static_cast<RenderThread*>(arg)->OnFrame();
// }

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

        // 创建渲染线程
        m_RenderThread = std::make_unique<RenderThread>(m_Window);
    }
#elif defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)
    Engine::Engine(EngineInitOpts initOpts)
        : m_InitOpts(initOpts), m_IsRunning(true)
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

        // 创建渲染线程
        if (initOpts.usingOffscreenCanvas) {
            m_RenderThread = std::make_unique<RenderThread>(m_Window);
        }
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
    // HtmlWindow* htmlwindow = static_cast<HtmlWindow*>(m_Window->GetNativeWindow());
    // while (!htmlwindow->ShouleClose())
    // {

    // }
            
    // EM_ASM(noExitRuntime=true);

    // emscripten_set_main_loop([](){
    //     // std::cout << "Rendering..." << std::endl;
    // }, 0, false);
#endif
}

void Engine::OnFrame()
{
    // Wait until swapchain is updated
    if (m_InitOpts.usingOffscreenCanvas) m_Window->GetSwapchainState();

    m_Scene->Render();

    // Reset SwapchainUpdated flag
    if (m_InitOpts.usingOffscreenCanvas) m_Window->SetSwapchainState(false);
}

void Engine::Render()
{
    std::cout << "Start sub thread for rendering, init..." << std::endl;

	// 在调用栈上，实例化一个OpenGL上下文对象
  	// // 在调用栈上，实例化一个WebGPU设备对象
    // // 渲染相关的API调用需要放到渲染线程中。
    // glfwMakeContextCurrent(m_Window);

    // // gladLoadGL(glfwGetProcAddress);
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //     std::cout << "Failed to initialize GLAD" << std::endl;
    //     return false;
    // }
    // glfwSwapInterval(1);

    if (m_Window == nullptr)
    {
        std::cerr << "Error: An valid \"Window\" instance is required." << std::endl;
        return;
    }

    // 在调用栈上，实例化一个WebGPU设备对象
	WebGPUDevice webGPUDevice;
    webGPUDevice.InitDevice(m_Window.get());

	if (m_Scene == nullptr)
    {
        std::cerr << "Error: An valid \"Scene\" instance is required." << std::endl;
        return;
    }

#if defined(IE_WGPU_NATIVE)

    while (m_IsRunning)
    {
        // if (webGPUDevice.IsInitialized())
        // {
                OnFrame();
        // }
    }

#elif defined(IE_ONLY_EMSCRIPTEN) || defined(IE_WGPU_EMSCRIPTEN)
    
    emscripten_set_main_loop_arg(
        [](void *userData) {
            Engine* engine = static_cast<Engine*>(userData);
            engine->OnFrame();
		},
		(void*)this, 0, true);  // 似乎必须是true

    // emscripten_set_main_loop_arg(&RenderLoopCallback, this, 0, false);
    
#endif

    m_Scene->Clear();

    WebGPUDevice::Get().Destroy();
}

void Engine::AddScene(std::shared_ptr<Scene> scene, bool active)
{
    m_Scene = scene;

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

        if (m_InitOpts.usingOffscreenCanvas) {
            // 创建渲染线程
            // m_RenderThread = std::make_unique<RenderThread>(m_Window);
            m_RenderThread->Start([](void*) -> void* {
                Engine::Get().Render();
                return nullptr;
            });
        }
        else {
            Render();
        }
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
        m_Scene = iter->second;

    #ifdef IE_GLFW_WINDOW
        GlfwWindow& glfwWindow = static_cast<GlfwWindow&>(m_Window->Get());
        glfwWindow.RegisterEvent();
        glfwWindow.SetScene(m_Scene);
    #elif defined(IE_HTML_WINDOW)
        HtmlWindow& htmlWindow = static_cast<HtmlWindow&>(m_Window->Get());
        htmlWindow.RegisterEvent();
        htmlWindow.SetScene(m_Scene);
    #endif
        // 如果渲染线程正在运行，则先停止渲染线程
        //if (m_RenderThread->IsRunning()) m_RenderThread->Stop();

        // 创建渲染线程
        // m_RenderThread = std::make_unique<RenderThread>(m_Window);

        //
        Run();
    }
}