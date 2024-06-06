#include "RenderThread.h"
#include "renderers/webgpu/WebGPUDevice.h"
#include "Common.h"

#ifdef IE_HTML_WINDOW
	#include "windows/HtmlWindow.h"
    #include "renderers/opengl/OpenGLRenderer.h"

    #ifdef IE_ONLY_EMSCRIPTEN
        #include "renderers/webgpu/WebGPURenderer.h"
    #endif

    #include <emscripten/threading.h>
#endif


//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/polar_coordinates.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <exception>


class ThreadException : public std::exception
{
public:
    ThreadException(const std::string &err)
        : m_Error(err)
    {
    }

    virtual const char *what() const noexcept override
    {
        return m_Error.c_str();
    }

private:
    std::string m_Error;
};

// temp var for OpenGLRenderer and WebGPURenderer test
static InitArgs initArgs{};

RenderThread::RenderThread(const std::unique_ptr<Window>& window)
    : m_Window(window.get()), m_Scene(nullptr), m_IsRunning(true)
{
// #if defined(IE_WGPU_NATIVE)
//     m_RenderThread = std::thread(&RenderThread::RenderMain, this);	//创建渲染线程，并指定渲染函数入口。
//     m_RenderThread.detach();
// #elif defined(IE_ONLY_EMSCRIPTEN) || defined(IE_WGPU_EMSCRIPTEN)
//     if (!emscripten_supports_offscreencanvas())
//     {
//         std::cout << "Current browser does not support OffscreenCanvas. Skipping the rest of the tests." << std::endl;
// #ifdef REPORT_RESULT
//         REPORT_RESULT(1);
// #endif
//     }

//     HtmlWindow* htmlWindow = static_cast<HtmlWindow*>(m_Window->GetNativeWindow());
//     // const char* canvasId = std::string("#").append(htmlWindow->GetCanvasId()).c_str();
//     std::string canvasId = std::string("#").append(htmlWindow->GetCanvasId());
//     std::cout << "48:canvasId-->" << canvasId << std::endl;

//     // 设置线程中OffscreenCanvas对象的属性
//     pthread_attr_t attr;
//     pthread_attr_init(&attr);
//     // emscripten_pthread_attr_settransferredcanvases(&attr, g_pHwd.c_str());
//     emscripten_pthread_attr_settransferredcanvases(&attr, canvasId.c_str());

//     //
//     // not working
//     pthread_create(&m_RenderThread, &attr, [](void* arg) -> void* {
//         RenderThread* renderThread = static_cast<RenderThread*>(arg);  
//         try
//         {
//             // 调用成员函数
//             renderThread->RenderMain();  
//         }
//         catch (ThreadException &e)
//         {
//             std::cout << "Catched the exception of rendering thread:" << e.what() << std::endl;
//         }
//         return nullptr;  
//     }, (void*)this);

//     //
//     // initArgs.window = nullptr;
//     // initArgs.canvasId = canvasId;
//     // initArgs.width = m_Window->GetWidth();
//     // initArgs.height = m_Window->GetHeight();

//     // working
//     // std::cout << "Using WebGL2 backend." << std::endl;
//     // pthread_create(&thread, &attr, initOpenGLRenderer, (void*)&initArgs);

//     // working
//     // std::cout << "Using WebGPU backend." << std::endl;
//     // pthread_create(&thread, &attr, initWebGPURenderer, (void*)&initArgs);

//     // 将渲染线程作为后台线程启动
//     // 使用该启动方式，只有主进程停止了，渲染线程才会停止
//     pthread_detach(m_RenderThread);
// #endif
}

RenderThread::~RenderThread()
{
#if defined(IE_WGPU_NATIVE)
    if (m_RenderThread.joinable()) {
        m_RenderThread.join();	//等待渲染线程结束
    }
#endif
}

void RenderThread::Start()
{
#if defined(IE_WGPU_NATIVE)
    m_RenderThread = std::thread(&RenderThread::RenderMain, this);	//创建渲染线程，并指定渲染函数入口。
    m_RenderThread.detach();
#elif defined(IE_ONLY_EMSCRIPTEN) || defined(IE_WGPU_EMSCRIPTEN)
    if (!emscripten_supports_offscreencanvas())
    {
        std::cout << "Current browser does not support OffscreenCanvas. Skipping the rest of the tests." << std::endl;
#ifdef REPORT_RESULT
        REPORT_RESULT(1);
#endif
    }

    HtmlWindow* htmlWindow = static_cast<HtmlWindow*>(m_Window->GetNativeWindow());
    std::string canvasId = std::string("#").append(htmlWindow->GetCanvasId());

    // 设置线程中OffscreenCanvas对象的属性
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    emscripten_pthread_attr_settransferredcanvases(&attr, canvasId.c_str());

    // Test01: WebGPURenderer(Use the engine architecture in this project) with the arch of (working!)
    pthread_create(&m_RenderThread, &attr, [](void* arg) -> void* {
        RenderThread* renderThread = static_cast<RenderThread*>(arg);  
        try
        {
            // 调用成员函数
            renderThread->RenderMain();  
        }
        catch (ThreadException &e)
        {
            std::cout << "Catched the exception of rendering thread:" << e.what() << std::endl;
        }
        return nullptr;  
    }, (void*)this);


    // Test02: OpenGLRenderer (working!)
    // initArgs.window = nullptr;
    // initArgs.canvasId = canvasId;
    // initArgs.width = m_Window->GetWidth();
    // initArgs.height = m_Window->GetHeight();
    // std::cout << "Using WebGL2 backend." << std::endl;
    // pthread_create(&m_RenderThread, &attr, initOpenGLRenderer, (void*)&initArgs);

    // Test03: WebGPURenderer (working!)
    // initArgs.window = nullptr;
    // initArgs.canvasId = canvasId;
    // initArgs.width = m_Window->GetWidth();
    // initArgs.height = m_Window->GetHeight();
    // std::cout << "Using WebGPU backend." << std::endl;
    // pthread_create(&m_RenderThread, &attr, initWebGPURenderer, (void*)&initArgs);
    
    // 将渲染线程作为后台线程启动
    // 使用该启动方式，只有主进程停止了，渲染线程才会停止
    pthread_detach(m_RenderThread);
#endif
}

void RenderThread::OnFrame()
{
    // Wait until swapchain is updated
    m_Window->GetSwapchainState();

    m_Scene->Render();

    // Reset SwapchainUpdated flag
    m_Window->SetSwapchainState(false);
}

void RenderThread::RenderMain()
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
    webGPUDevice.InitDevice(m_Window);

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
			RenderThread* renderThread = static_cast<RenderThread*>(userData);
			renderThread->OnFrame();
		},
		(void*)this, 0, true);

    // emscripten_set_main_loop_arg(&RenderLoopCallback, this, 0, false);
    
#endif

    m_Scene->Clear();

    WebGPUDevice::Get().Destroy();
}