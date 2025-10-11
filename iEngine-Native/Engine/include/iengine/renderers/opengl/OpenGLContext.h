#pragma once

#include "../Context.h"
#include <memory>

struct GLFWwindow;

namespace iengine {
    struct OpenGLContextOptions {
        bool useOpenGL33 = false;
        bool useOpenGL41 = false;
        bool useOpenGL43 = false;
        bool useOpenGL45 = true;
    };
    
    class OpenGLContext : public Context {
    public:
        OpenGLContext(GLFWwindow* window, const OpenGLContextOptions& options = OpenGLContextOptions{});
        ~OpenGLContext();
        
        void init();
        void clear() override;
        void resize(int width, int height) override;
        
        // Buffer操作
        void* createVertexBuffer(size_t size) override;
        void* createIndexBuffer(size_t size) override;
        void deleteBuffer(void* buffer) override;
        void writeBuffer(void* buffer, const void* data, size_t size, size_t offset = 0) override;
        
        // 绘制操作
        void draw(std::shared_ptr<class Mesh> mesh) override;
        
        void* getDevice() const { return device_; }
        
        int getWidth() const { return width_; }
        int getHeight() const { return height_; }
        int getDisplayWidth() const { return displayWidth_; }
        int getDisplayHeight() const { return displayHeight_; }
        
    private:
        GLFWwindow* window_ = nullptr;
        void* device_ = nullptr; // OpenGL上下文
        
        OpenGLContextOptions options_;
        
        int width_ = 800;
        int height_ = 600;
        int displayWidth_ = 800;
        int displayHeight_ = 600;
    };
}