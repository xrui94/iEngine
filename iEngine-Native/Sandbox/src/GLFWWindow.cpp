#include "GLFWWindow.h"
#include <glad/glad.h>  // 必须在GLFW之前包含glad
#include <iengine/iengine.h>
#include <iengine/renderers/opengl/OpenGLContext.h>
#include <iostream>

namespace sandbox {

    GLFWWindow::GLFWWindow()
        : window_(nullptr)
        , context_(nullptr)
        , eventCallback_(nullptr) {
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
        if (self && self->eventCallback_) {
            // 可以通过eventCallback_通知Engine窗口大小改变
            std::cout << "窗口大小调整为: " << width << "x" << height << std::endl;
        }
        glViewport(0, 0, width, height);
    }

    void GLFWWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (self && self->eventCallback_) {
            // 可以通过eventCallback_通知Engine键盘事件
        }
    }

    void GLFWWindow::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        if (self && self->eventCallback_) {
            // 可以通过eventCallback_通知Engine鼠标按钮事件
        }
    }

    void GLFWWindow::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        if (self && self->eventCallback_) {
            // 可以通过eventCallback_通知Engine鼠标移动事件
        }
    }

} // namespace sandbox
