#include "Engine.h"
#include "Scene.h"
#include "Mesh.h"
#include "cameras/PerspectiveCamera.h"
#include "VertexAttrs.h"

#if defined(__EMSCRIPTEN__)
    #include <emscripten.h>
    #include <emscripten/bind.h>
#else
    #include "windows/GlfwWindow.h"
#endif

#include <stdint.h>
#include <iostream>
#include <memory>

#if defined(__EMSCRIPTEN__)

    // struct StartupOpts
    // {
    //     // StartupOpts(std::string containerId,uint32_t width, uint32_t height) 
    //     //     : containerId(containerId), width(width), height(height),
    //     //     backend("webgpu"), usingOffscreenCanvas(true), 
    //     //     customCanvas(false), canvasId("#canvas"), style("")
    //     // {
    //     // }

    //     std::string containerId;
    //     uint32_t width;
    //     uint32_t height;
    //     std::string backend = "webgpu";
    //     bool usingOffscreenCanvas = true;
    //     bool customCanvas = false;
    //     std::string canvasId = "#manvas";
    //     std::string style = "";
    // };

    // Binding code
    EMSCRIPTEN_BINDINGS(iEngine) {

        //
        emscripten::value_object<EngineInitOpts>("EngineInitOpts")
            .field("containerId", &EngineInitOpts::containerId)
            .field("width", &EngineInitOpts::width)
            .field("height", &EngineInitOpts::height)
            // .field("backend", &EngineInitOpts::backend)
            .field("usingOffscreenCanvas", &EngineInitOpts::usingOffscreenCanvas)
            // .field("customCanvas", &EngineInitOpts::customCanvas)
            .field("canvasId", &EngineInitOpts::canvasId)
            .field("style", &EngineInitOpts::style)
        ;

        //
        emscripten::value_object<WebVertexAttrInfo>("WebVertexAttrInfo")
            .field("location", &WebVertexAttrInfo::location)
            .field("format", &WebVertexAttrInfo::format)
            .field("offset", &WebVertexAttrInfo::offset);
    
        emscripten::register_vector<WebVertexAttrInfo>("WebVertexAttrInfo");
    
        emscripten::class_<Engine>("Engine")
            // .constructor<>() // 构造函数
            // .smart_ptr<std::shared_ptr<Engine>>("Engine")
            .smart_ptr_constructor("Engine", &std::make_shared<Engine, EngineInitOpts>)
            // .function("init", &Engine::Init) // 普通类成员函数
            .function("addScene", &Engine::AddScene) // 普通类成员函数
            ;

        emscripten::class_<Scene>("Scene")
            // .constructor<std::shared_ptr<Engine>>() // 构造函数
            .smart_ptr_constructor("Scene", &std::make_shared<Scene, uint32_t, uint32_t, std::shared_ptr<PerspectiveCamera>>)
            // .smart_ptr<std::shared_ptr<Scene>>("Scene")
            // .smart_ptr_constructor("Scene", &std::make_shared<Scene>())
            .function("addCamera", &Scene::AddCamera) // 普通类成员函数
            .function("addMesh", &Scene::AddMesh) // 普通类成员函数
            ;

        emscripten::class_<Mesh>("Mesh")
            .smart_ptr_constructor("Mesh", &std::make_shared<Mesh, emscripten::val, emscripten::val, emscripten::val>)
            ;

        emscripten::class_<PerspectiveCamera>("PerspectiveCamera")
            .smart_ptr_constructor("PerspectiveCamera", &std::make_shared<PerspectiveCamera, float>)
            ;
    }

#else
    static int32_t width = 960;
    static int32_t height = 640;

    int main(int, char**)
    {
        // 设置控制台输出编码为UTF-8
        system("chcp 65001");

        // 模拟Mesh数据（这里以三角形为例）
        // std::vector<VertexAttrs> vertexBuffer =  {
        //     {
        //         glm::vec3(-0.5f, -0.5f, 0.0f), 
        //         glm::vec3(0.0f, 0.f, 1.f),
        //         glm::vec3(1.f, 0.f, 0.f),
        //     },
        //     {
        //         glm::vec3(0.5f, -0.5f, 0.0f), 
        //         glm::vec3(0.0f, 0.f, 1.f),
        //         glm::vec3(0.f, 1.f, 0.f),
        //     },
        //     {
        //         glm::vec3(0.f, 0.5f, 0.0f), 
        //         glm::vec3(0.0f, 0.f, 1.f),
        //         glm::vec3(0.f, 0.f, 1.f),
        //     }
        // };

        std::vector<float> vertexBuffer =  {
            // position             // normal        // color
            -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 1.0f,     0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,
             0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,
        };

        // index
        std::vector<uint32_t> indices = {0, 1, 2};

        std::vector<VertexAttrInfo> vertexAttrInfoList = {
            {
                0,
                // glGetAttribLocation(program, "a_pos"),
                wgpu::VertexFormat::Float32x3,
                0
            },
            {
                1,
                // glGetAttribLocation(program, "a_pos"),
                wgpu::VertexFormat::Float32x3,
                offsetof(VertexAttrs, normal)
            },
            {
                2,
                // glGetAttribLocation(program, "a_pos"),
                wgpu::VertexFormat::Float32x3,
                offsetof(VertexAttrs, color)
            }
        };

        // 创建一个引擎实例
        EngineInitOpts initOpts{};
        initOpts.containerId = "glfw-wgpu";
        initOpts.width = 960;
        initOpts.height = 640;
        initOpts.usingOffscreenCanvas = true;
        Engine& engine = Engine::GetInstance(initOpts);

        // 创建一个Mesh
        auto mesh = std::make_shared<Mesh>(
            vertexBuffer,
            indices,
            vertexAttrInfoList
        );

        // 创建一个透视相机
        auto perspectiveCamera = std::make_shared<PerspectiveCamera>(width / static_cast<float>(height));

        //// 创建一个场景
        auto scene = std::make_shared<Scene>(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

        // 设置场景属性
        scene->AddCamera(perspectiveCamera);
        scene->AddMesh(mesh);

        // 将场景添加到引擎实例中
        engine.AddScene(scene);

        exit(EXIT_SUCCESS);
    }
#endif  // __EMSCRIPTEN__

// #Ref:
// WebAssembly初级——Embind普通函数、C++类（四）：https://blog.csdn.net/qq_42956179/article/details/118031830
// How to compile C++ classes to .wasm files for wasmer：https://stackoverflow.com/questions/71462530/how-to-compile-c-classes-to-wasm-files-for-wasmer
// 智能指针传递对象：https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html
// https://github.com/beaufortfrancois/webgpu-cross-platform-app/blob/main/main.cpp