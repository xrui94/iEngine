#pragma once

#include <iengine/windowing/Window.h>
#include <glad/glad.h>  // 必须在GLFW之前包含glad
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

namespace sandbox {

    /**
     * @brief GLFW窗口实现，符合Engine的WindowInterface接口
     * 
     * 这个类将GLFW窗口封装为Engine可以使用的窗口抽象，
     * 实现了窗口大小查询、关闭状态检查、上下文管理等功能。
     */
    class GLFWWindow : public iengine::WindowInterface {
    public:
        GLFWWindow();
        ~GLFWWindow() override;

        /**
         * @brief 初始化GLFW窗口
         * @param width 窗口宽度
         * @param height 窗口高度
         * @param title 窗口标题
         * @return 初始化是否成功
         */
        bool initialize(int width, int height, const std::string& title);

        // WindowInterface 接口实现
        void getSize(int& width, int& height) const override;
        bool shouldClose() const override;
        std::shared_ptr<iengine::Context> getContext() const override;
        void makeContextCurrent() override;
        void setEventCallback(const iengine::WindowEventCallback& callback) override;

        // GLFW特有的方法
        GLFWwindow* getGLFWHandle() const { return window_; }
        void pollEvents();
        void swapBuffers();

    private:
        GLFWwindow* window_;
        std::shared_ptr<iengine::Context> context_;
        iengine::WindowEventCallback eventCallback_;

        // 静态GLFW回调函数
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    };

} // namespace sandbox
