#pragma once

#include <memory>

namespace iengine {
    class Context {
    public:
        virtual ~Context() = default;
        
        virtual void clear() = 0;
        virtual void resize(int width, int height) = 0;
        
        // Buffer操作
        virtual void* createVertexBuffer(size_t size) = 0;
        virtual void* createIndexBuffer(size_t size) = 0;
        virtual void deleteBuffer(void* buffer) = 0;
        virtual void writeBuffer(void* buffer, const void* data, size_t size, size_t offset = 0) = 0;
        
        // 绘制操作
        virtual void draw(std::shared_ptr<class Mesh> mesh) = 0;
    };
}