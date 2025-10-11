#pragma once

#include <memory>
#include <string>
#include <map>
#include "../renderers/Renderer.h"

// 前向声明
namespace iengine {
    class Scene;
    class Context;
    
    struct EngineOptions {
        RendererType renderer = RendererType::OpenGL;
        bool disableWebGPU = false;
    };
    
    class Engine {
    public:
        Engine(const EngineOptions& options = EngineOptions{});
        ~Engine();
        
        void start();
        void stop();
        
        void addScene(const std::string& name, std::shared_ptr<Scene> scene);
        void setActiveScene(const std::string& name);
        std::shared_ptr<Scene> getScene(const std::string& name);
        
        void setRenderer(RendererType renderer);
        
        // 公共渲染方法，供外部调用
        void render();
        
    private:
        void initRenderer();
        void update(float deltaTime);
        void tick();
        
        std::unique_ptr<Renderer> activeRenderer_;
        std::unique_ptr<Renderer> openglRenderer_;
        std::unique_ptr<Renderer> webgpuRenderer_;
        
        std::map<std::string, std::shared_ptr<Scene>> scenes_;
        std::shared_ptr<Scene> activeScene_;
        
        bool running_ = false;
        float lastTime_ = 0.0f;
    };
}