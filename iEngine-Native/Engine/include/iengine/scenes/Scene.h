#pragma once

#include <memory>
#include <vector>
#include "../renderers/Renderer.h"
#include "../windowing/Window.h"

// 前向声明
namespace iengine {
    class Camera;
    class Light;
    class Model;
    class Context;
    class OpenGLContext;
    
    class Scene {
    public:
        Scene(std::shared_ptr<WindowInterface> window);
        ~Scene();
        
        void addComponent(std::shared_ptr<Model> component);
        void removeComponent(std::shared_ptr<Model> component);
        const std::vector<std::shared_ptr<Model>>& getComponents() const;
        
        void addLight(std::shared_ptr<Light> light);
        void removeLight(std::shared_ptr<Light> light);
        const std::vector<std::shared_ptr<Light>>& getLights() const;
        
        void update(float deltaTime);
        
        std::shared_ptr<Camera> getActiveCamera() const;
        void setActiveCamera(std::shared_ptr<Camera> camera);
        
        // Context相关
        std::shared_ptr<Context> getContext() const;
        void setContextType(RendererType type);
        
    private:
        std::vector<std::shared_ptr<Model>> components_;
        std::vector<std::shared_ptr<Light>> lights_;
        std::shared_ptr<Camera> activeCamera_;
        
        // Context管理
        std::shared_ptr<WindowInterface> window_;
        std::shared_ptr<Context> activeContext_;
        std::shared_ptr<OpenGLContext> openglContext_;
        // 未来可以添加WebGPU Context
        // std::shared_ptr<WebGPUContext> webgpuContext_;
    };
}