#include <iengine/iengine.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>  // GLFW必须在glad之后包含
#include "GLFWWindow.h"
#include <iostream>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

// 简单的GLFW + iEngine应用程序
namespace sandbox {
    
    // GLFW应用程序类
    class GLFWApplication {
    public:
        GLFWApplication() : window_(nullptr), engine_(nullptr), scene_(nullptr) {}
        
        ~GLFWApplication() {
            cleanup();
        }
        
        bool initialize() {
            std::cout << "初始化GLFW + iEngine应用程序..." << std::endl;
            
            // 设置错误回调
            glfwSetErrorCallback([](int error, const char* description) {
                std::cerr << "GLFW Error " << error << ": " << description << std::endl;
            });
            
            // 初始化GLFW
            if (!glfwInit()) {
                std::cerr << "Failed to initialize GLFW" << std::endl;
                return false;
            }
            
            // 设置OpenGL版本和配置
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            
            #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            #endif
            
            // 创建使用Engine窗口抽象的GLFW窗口
            window_ = std::make_shared<GLFWWindow>();
            if (!window_->initialize(1024, 768, "iEngine GLFW Example")) {
                return false;
            }
            
            // 启用垂直同步
            glfwSwapInterval(1);
            
            // 注册GLFW窗口创建器到Engine的WindowFactory
            iengine::WindowFactory::registerWindowCreator(
                iengine::WindowType::GLFW,
                []() -> std::unique_ptr<iengine::WindowInterface> {
                    auto window = std::make_unique<GLFWWindow>();
                    if (window->initialize(1024, 768, "iEngine GLFW Window")) {
                        return window;
                    }
                    return nullptr;
                }
            );
            
            std::cout << "GLFW窗口已注册到Engine的WindowFactory" << std::endl;
            
            // 创建引擎
            try {
                iengine::EngineOptions options;
                options.renderer = iengine::RendererType::OpenGL;
                engine_ = std::make_shared<iengine::Engine>(options);
                
                std::cout << "iEngine初始化成功" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Failed to initialize iEngine: " << e.what() << std::endl;
                return false;
            }
            
            // 设置场景
            if (!setupScene()) {
                std::cerr << "Failed to setup scene" << std::endl;
                return false;
            }

            engine_->start();
            
            std::cout << "GLFW + iEngine应用程序初始化成功" << std::endl;
            std::cout << "提示: 按ESC键退出程序" << std::endl;
            return true;
        }
        
        void run() {
            if (!initialize()) {
                return;
            }
            
            std::cout << "启动GLFW + iEngine应用程序主循环..." << std::endl;
            
            int frameCount = 0;
            auto lastTime = glfwGetTime();
            
            while (!window_->shouldClose()) {
                // 处理事件
                window_->pollEvents();
                
                // 渲染
                render();
                
                // 交换缓冲区
                window_->swapBuffers();
                
                frameCount++;
                
                // 每秒输出一次FPS信息
                auto currentTime = glfwGetTime();
                if (currentTime - lastTime >= 1.0) {
                    std::cout << "FPS: " << frameCount << std::endl;
                    frameCount = 0;
                    lastTime = currentTime;
                }
            }
            
            std::cout << "GLFW + iEngine应用程序主循环结束" << std::endl;
        }
        
    private:
        std::shared_ptr<GLFWWindow> window_;
        std::shared_ptr<iengine::Engine> engine_;
        std::shared_ptr<iengine::Scene> scene_;
        std::shared_ptr<iengine::PerspectiveCamera> camera_;
        
        bool setupScene() {
            if (!engine_) {
                return false;
            }
            
            try {
                std::cout << "创建场景..." << std::endl;
                
                // 创建场景，传入GLFW窗口接口
                scene_ = std::make_shared<iengine::Scene>(window_);
                
                // 创建相机
                camera_ = std::make_shared<iengine::PerspectiveCamera>(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
                camera_->setPosition(0.0f, 0.0f, 5.0f);
                camera_->lookAt(0.0f, 0.0f, 0.0f);
                scene_->setActiveCamera(camera_);
                
                // 创建几何体 - 立方体
                auto cubeGeometry = std::make_shared<iengine::Cube>(1.0f);
                
                // 创建图元
                auto cubePrimitive = std::make_shared<iengine::Primitive>(iengine::PrimitiveType::TRIANGLES);
                
                // 创建网格
                auto cubeMesh = std::make_shared<iengine::Mesh>(cubeGeometry, cubePrimitive);
                
                // 创建材质
                iengine::BaseMaterialParams params;
                params.name = "GlfwCubeMaterial";
                params.shaderName = "base_material";  // 使用正确的着色器名称
                params.color = iengine::Color(1.0f, 0.5f, 0.2f, 1.0f); // 橙色立方体
                auto material = std::make_shared<iengine::BaseMaterial>(params);
                
                // 创建模型
                auto cubeModel = std::make_shared<iengine::Model>("GlfwCube", cubeMesh, material);
                
                // 将模型添加到场景
                scene_->addComponent(cubeModel);
                
                // 将场景添加到引擎
                engine_->addScene("main", scene_);
                
                std::cout << "场景创建成功，包含 " << scene_->getComponents().size() << " 个组件" << std::endl;
                
                // 注册内置着色器
                //iengine::ShaderLib::registerBuiltInShaders();
                auto shaderNames = iengine::ShaderLib::getAllShaderNames();
                std::cout << "已注册 " << shaderNames.size() << " 个着色器" << std::endl;
                
                return true;
                
            } catch (const std::exception& e) {
                std::cerr << "场景设置失败: " << e.what() << std::endl;
                return false;
            }
        }
        
        void render() {
            // 确保OpenGL上下文是当前的
            window_->makeContextCurrent();
            
            // 使用iEngine进行渲染
            if (engine_ && scene_) {
                try {
                    // 调用引擎渲染
                    engine_->render();
                } catch (const std::exception& e) {
                    std::cerr << "Engine render error: " << e.what() << std::endl;
                    // 如果引擎渲染失败，使用备用渲染
                    fallbackRender();
                }
            } else {
                fallbackRender();
            }
        }
        
        void fallbackRender() {
            // 备用渲染：简单的清屏
            glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        
        void cleanup() {
            if (scene_) {
                scene_.reset();
            }
            if (engine_) {
                engine_.reset();
            }
            if (window_) {
                window_.reset();
            }
            glfwTerminate();
        }
    };
    
} // namespace sandbox

// 设置控制台编码以支持中文输出
void setupConsoleEncoding() {
#ifdef _WIN32
    // 设置控制台输出编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 设置cout和cerr使用UTF-8
    std::locale::global(std::locale(".UTF8"));
#endif
}

// GLFW + iEngine应用程序测试的main函数
int main() {
    // 首先设置控制台编码
    setupConsoleEncoding();
    
    try {
        std::cout << "=== iEngine GLFW + OpenGL 应用程序示例 ===" << std::endl;
        
        sandbox::GLFWApplication app;
        app.run();
        
        std::cout << "GLFW + iEngine应用程序示例完成" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "GLFW + iEngine应用程序异常: " << e.what() << std::endl;
        return -1;
    }
}