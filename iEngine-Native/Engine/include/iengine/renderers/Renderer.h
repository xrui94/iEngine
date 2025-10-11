#pragma once

#include <memory>

// 前向声明
namespace iengine {
    class Scene;
    
    enum class RendererType {
        OpenGL,
        WebGPU
    };
    
    class Renderer {
    public:
        virtual ~Renderer() = default;
        
        virtual bool initialize() = 0;
        virtual void cleanup() = 0;
        virtual void render(std::shared_ptr<Scene> scene) = 0;
        virtual void resize(int width, int height) = 0;
        virtual void clear() = 0;
        
        static bool isRendererType(RendererType type) {
            return type == RendererType::OpenGL || type == RendererType::WebGPU;
        }
    };
}