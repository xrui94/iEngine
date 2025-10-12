#pragma once

#include <memory>

// 前向声明
namespace iengine {
    class Scene;
    class Context;
    
    enum class RendererType {
        OpenGL,
        WebGPU
    };
    
    class Renderer {
    public:
        virtual ~Renderer() = default;
        
        virtual bool initialize(std::shared_ptr<Context> context) = 0;
        virtual void cleanup() = 0;
        virtual void render(std::shared_ptr<Scene> scene) = 0;
        virtual void resize(int width, int height) = 0;
        virtual void clear() = 0;
        
        static bool isRendererType(RendererType type) {
            return type == RendererType::OpenGL || type == RendererType::WebGPU;
        }
    };
}