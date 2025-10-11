#include "../../include/iengine/core/Engine.h"

#include <iostream>
#include <memory>
#include "../include/iengine/scenes/Scene.h"
#include "../include/iengine/renderers/opengl/OpenGLRenderer.h"

#ifdef IENGINE_WEBGPU_SUPPORT
#include "../include/iengine/renderers/webgpu/WebGPURenderer.h"
#endif

namespace iengine {

    Engine::Engine(const EngineOptions& options) {
        setRenderer(options.renderer);
    }

    Engine::~Engine() {
        stop();
    }

    void Engine::start() {
        if (running_) {
            return;
        }

        running_ = true;
        lastTime_ = 0.0f;

        // 初始化渲染器
        if (activeRenderer_) {
            activeRenderer_->initialize();
        }

        // 启动主循环
        while (running_) {
            tick();
        }
    }

    void Engine::stop() {
        if (!running_) {
            return;
        }

        running_ = false;

        // 清理渲染器
        if (activeRenderer_) {
            activeRenderer_->cleanup();
        }
    }

    void Engine::addScene(const std::string& name, std::shared_ptr<Scene> scene) {
        scenes_[name] = scene;
    }

    void Engine::setActiveScene(const std::string& name) {
        auto it = scenes_.find(name);
        if (it != scenes_.end()) {
            activeScene_ = it->second;
        }
    }

    std::shared_ptr<Scene> Engine::getScene(const std::string& name) {
        auto it = scenes_.find(name);
        if (it != scenes_.end()) {
            return it->second;
        }
        return nullptr;
    }

    void Engine::setRenderer(RendererType renderer) {
        switch (renderer) {
            case RendererType::OpenGL:
                if (!openglRenderer_) {
                    openglRenderer_ = std::make_unique<OpenGLRenderer>();
                }
                activeRenderer_ = std::move(openglRenderer_);
                break;
#ifdef IENGINE_WEBGPU_SUPPORT
            case RendererType::WebGPU:
                if (!webgpuRenderer_) {
                    webgpuRenderer_ = std::make_unique<WebGPURenderer>();
                }
                activeRenderer_ = std::move(webgpuRenderer_);
                break;
#endif
            default:
                // 默认使用 OpenGL
                if (!openglRenderer_) {
                    openglRenderer_ = std::make_unique<OpenGLRenderer>();
                }
                activeRenderer_ = std::move(openglRenderer_);
                break;
        }
    }

    void Engine::initRenderer() {
        if (activeRenderer_) {
            activeRenderer_->initialize();
        }
    }

    void Engine::update(float deltaTime) {
        // 更新场景
        if (activeScene_) {
            activeScene_->update(deltaTime);
        }
    }

    void Engine::render() {
        if (activeRenderer_ && activeScene_) {
            activeRenderer_->render(activeScene_);
        }
    }

    void Engine::tick() {
        // 计算时间差
        // 这里应该使用实际的时间函数
        float currentTime = 0.0f; // 应该从系统获取当前时间
        float deltaTime = currentTime - lastTime_;
        lastTime_ = currentTime;

        // 更新逻辑
        update(deltaTime);

        // 渲染
        render();
    }

} // namespace iengine