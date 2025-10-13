#include "GLFWWindow.h"
#include <glad/glad.h>  // 必须在GLFW之前包含glad
#include <iengine/iengine.h>
#include <iengine/renderers/opengl/OpenGLContext.h>
#include <iostream>

namespace sandbox {

    GLFWWindow::GLFWWindow()
        : window_(nullptr)
        , context_(nullptr)
        , eventCallback_(nullptr)
        , eventDispatcher_(std::make_unique<iengine::WindowEventDispatcher>()) {
    }

    GLFWWindow::~GLFWWindow() {
        if (window_) {
            glfwDestroyWindow(window_);
            window_ = nullptr;
        }
    }

    bool GLFWWindow::initialize(int width, int height, const std::string& title) {
        // 创建GLFW窗口
        window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!window_) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            return false;
        }

        // 设置用户指针，用于回调
        glfwSetWindowUserPointer(window_, this);

        // 设置回调函数
        glfwSetFramebufferSizeCallback(window_, framebufferSizeCallback);
        glfwSetKeyCallback(window_, keyCallback);
        glfwSetMouseButtonCallback(window_, mouseButtonCallback);
        glfwSetCursorPosCallback(window_, cursorPosCallback);

        // 设置OpenGL上下文
        makeContextCurrent();

        // 初始化GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        // 注意：不在这里创建 OpenGLContext，由 Scene 负责创建
        // Context 将通过 WindowInterface 接口获取必要信息

        std::cout << "GLFW窗口创建成功" << std::endl;
        std::cout << "OpenGL版本: " << glGetString(GL_VERSION) << std::endl;

        // 注册滚轮回调
        glfwSetScrollCallback(window_, [](GLFWwindow* w, double xoffset, double yoffset) {
            auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(w));
            if (self) {
                // 【新版本】使用观察者模式分发事件
                iengine::WindowEvent event;
                event.type = iengine::WindowEventType::MouseScroll;
                event.data.mouseScroll.xoffset = xoffset;
                event.data.mouseScroll.yoffset = yoffset;
                self->eventDispatcher_->dispatchEvent(event);
                
                // 【旧版本已弃用】通过回调函数分发事件（会导致双重处理）
                // if (self->eventCallback_) {
                //     self->eventCallback_(event);
                // }
            }
        });
        
        return true;
    }

    void GLFWWindow::getSize(int& width, int& height) const {
        if (window_) {
            glfwGetFramebufferSize(window_, &width, &height);
        } else {
            width = height = 0;
        }
    }

    bool GLFWWindow::shouldClose() const {
        return window_ ? glfwWindowShouldClose(window_) : true;
    }

    std::shared_ptr<iengine::Context> GLFWWindow::getContext() const {
        // Context 由 Scene 通过 WindowInterface 创建，这里返回 nullptr
        // Scene 将创建 OpenGLContext 并传入 shared_ptr<WindowInterface>
        return nullptr;
    }

    void GLFWWindow::makeContextCurrent() {
        if (window_) {
            glfwMakeContextCurrent(window_);
        }
    }

    void GLFWWindow::setEventCallback(const iengine::WindowEventCallback& callback) {
        eventCallback_ = callback;
    }
    
    iengine::WindowEventDispatcher& GLFWWindow::getEventDispatcher() {
        return *eventDispatcher_;
    }

    const iengine::WindowEventDispatcher& GLFWWindow::getEventDispatcher() const {
        return *eventDispatcher_;
    }

    void GLFWWindow::pollEvents() {
        glfwPollEvents();
    }

    void GLFWWindow::swapBuffers() {
        if (window_) {
            glfwSwapBuffers(window_);
        }
    }

    // 静态回调函数实现
    void GLFWWindow::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        if (self) {
            // 【新版本】使用观察者模式分发resize事件
            iengine::WindowEvent event;
            event.type = iengine::WindowEventType::Resize;
            event.data.resize.width = width;
            event.data.resize.height = height;
            self->eventDispatcher_->dispatchEvent(event);
            std::cout << "窗口大小调整为: " << width << "x" << height << ", 已通过观察者模式通知" << std::endl;
            
            // 【旧版本已弃用】通过eventCallback_通知（会导致双重处理）
            // if (self->eventCallback_) {
            //     self->eventCallback_(event);
            // }
        } else {
            std::cout << "窗口大小调整为: " << width << "x" << height << std::endl;
        }
        glViewport(0, 0, width, height);
    }

    void GLFWWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (self) {
            // 【新版本】使用观察者模式分发键盘事件
            iengine::WindowEvent event;
            event.type = iengine::WindowEventType::Key;
            event.data.key.key = key;
            
            // 映射GLFW动作到引擎枚举
            if (action == GLFW_PRESS) {
                event.data.key.action = iengine::KeyAction::Press;
            } else if (action == GLFW_RELEASE) {
                event.data.key.action = iengine::KeyAction::Release;
            } else if (action == GLFW_REPEAT) {
                event.data.key.action = iengine::KeyAction::Repeat;
            }
            
            event.data.key.mods = mods;
            self->eventDispatcher_->dispatchEvent(event);
            
            // 【旧版本已弃用】通过回调函数分发（会导致双重处理）
            // if (self->eventCallback_) {
            //     self->eventCallback_(event);
            // }
        }
    }

    void GLFWWindow::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        if (self) {
            // 【新版本】使用观察者模式分发鼠标按键事件
            iengine::WindowEvent event;
            event.type = iengine::WindowEventType::MouseButton;
                
            // 映射GLFW鼠标按键到引擎枚举
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                event.data.mouseButton.button = iengine::MouseButton::Left;
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                event.data.mouseButton.button = iengine::MouseButton::Right;
            } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                event.data.mouseButton.button = iengine::MouseButton::Middle;
            }
                
            // 映射动作
            if (action == GLFW_PRESS) {
                event.data.mouseButton.action = iengine::KeyAction::Press;
            } else if (action == GLFW_RELEASE) {
                event.data.mouseButton.action = iengine::KeyAction::Release;
            }
                
            // 获取鼠标位置
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            event.data.mouseButton.x = x;
            event.data.mouseButton.y = y;
                
            self->eventDispatcher_->dispatchEvent(event);
            
            // 【旧版本已弃用】通过回调函数分发（会导致双重处理）
            // if (self->eventCallback_) {
            //     self->eventCallback_(event);
            // }
        }
    }

    void GLFWWindow::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        if (self) {
            // 【新版本】使用观察者模式分发鼠标移动事件
            iengine::WindowEvent event;
            event.type = iengine::WindowEventType::MouseMove;
            event.data.mouseMove.x = xpos;
            event.data.mouseMove.y = ypos;
            self->eventDispatcher_->dispatchEvent(event);
            
            // 【旧版本已弃用】通过回调函数分发（会导致双重处理）
            // if (self->eventCallback_) {
            //     self->eventCallback_(event);
            // }
        }
    }

} // namespace sandbox
