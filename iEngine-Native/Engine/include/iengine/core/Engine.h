#pragma once

#include "../renderers/Renderer.h"

#include <memory>
#include <string>
#include <map>
#include <atomic>

// 前向声明
namespace iengine {
    class Scene;
    class Context;
    
    struct EngineOptions {
        RendererType renderer = RendererType::OpenGL;
        bool disableWebGPU = false;
    };
    
    /**
    * @brief 图形渲染引擎核心类
    * 
    * @note 推荐做法：一个应用程序只创建一个Engine实例
    * 
    * Engine 负责管理渲染器、场景和渲染循环。虽然技术上允许创建
    * 多个实例，但强烈不推荐这样做，原因如下：
    * 
    * 1. GPU上下文开销：每个实例可能创建独立的OpenGL/WebGPU上下文
    * 2. 资源浪费：着色器、纹理等资源无法跨实例共享
    * 3. 性能影响：多上下文切换会降低渲染性能
    * 
    * @code
    * // ✅ 推荐：单Engine多Scene模式
    * iengine::Engine engine(options);
    * engine.addScene("main", mainScene);
    * engine.addScene("ui", uiScene);
    * 
    * // ⚠️ 不推荐：创建多个Engine（会收到警告）
    * iengine::Engine engine1(options);  // OK
    * iengine::Engine engine2(options);  // WARNING: Multiple instances!
    * @endcode
    * 
    * @see Scene - 使用多个Scene来组织不同的渲染内容
    */
    class Engine {
    public:
        Engine(const EngineOptions& options = EngineOptions{});
        ~Engine();

        // 禁止拷贝（可选：允许移动）
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
        // 如果需要，可启用移动语义（通常不需要）
        // Engine(Engine&&) noexcept;
        // Engine& operator=(Engine&&) noexcept;

    public:
        void start();
        void stop();
        
        void addScene(const std::string& name, std::shared_ptr<Scene> scene);
        void setActiveScene(const std::string& name);
        bool removeScene(const std::string& name);
        bool hasScene(const std::string& name) const;
        std::shared_ptr<Scene> getActiveScene() const;
        std::shared_ptr<Scene> getScene(const std::string& name) const;
        
        void setRenderer(RendererType renderer);
        
        // 公共渲染方法，供外部调用
        void tick();
        //void render();
        
        // 新增：resize 事件处理（对齐 Web 版本）
        void resize(int width, int height);

        bool isReady() const noexcept;
        
    private:
        void initRenderer();
        void setRenderer(RendererType renderer, bool init);
        void update(float deltaTime);
        void render();
        //void tick();
        
    private:
        // 用于调试时检测多实例（非强制）
        static std::atomic<int> s_instanceCount;

        std::unique_ptr<Renderer> activeRenderer_;
        std::unique_ptr<Renderer> openglRenderer_;
        std::unique_ptr<Renderer> webgpuRenderer_;
        
        std::map<std::string, std::shared_ptr<Scene>> scenes_;
        std::shared_ptr<Scene> activeScene_;
        
        bool running_ = false;
        float lastTime_ = 0.0f;
    };
}