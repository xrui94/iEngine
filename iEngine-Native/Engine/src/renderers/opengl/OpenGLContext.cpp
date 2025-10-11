#include "../../../include/iengine/renderers/opengl/OpenGLContext.h"

namespace iengine {
    OpenGLContext::OpenGLContext(GLFWwindow* window, const OpenGLContextOptions& options)
        : window_(window), options_(options) {}
    
    OpenGLContext::~OpenGLContext() {}
    
    void OpenGLContext::init() {
        // 初始化OpenGL上下文
    }
    
    void OpenGLContext::clear() {
        // 清除屏幕
    }
    
    void OpenGLContext::resize(int width, int height) {
        width_ = width;
        height_ = height;
        displayWidth_ = width;
        displayHeight_ = height;
    }
    
    void* OpenGLContext::createVertexBuffer(size_t size) {
        // 创建顶点缓冲区
        return nullptr;
    }
    
    void* OpenGLContext::createIndexBuffer(size_t size) {
        // 创建索引缓冲区
        return nullptr;
    }
    
    void OpenGLContext::deleteBuffer(void* buffer) {
        // 删除缓冲区
    }
    
    void OpenGLContext::writeBuffer(void* buffer, const void* data, size_t size, size_t offset) {
        // 写入缓冲区数据
    }
    
    void OpenGLContext::draw(std::shared_ptr<class Mesh> mesh) {
        // 绘制网格
    }
}