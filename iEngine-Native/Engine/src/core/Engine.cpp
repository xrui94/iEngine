#include "iengine/core/Engine.h"
#include "iengine/scenes/Scene.h"
#include "iengine/renderers/opengl/OpenGLRenderer.h"
#include "iengine/shaders/ShaderLib.h"
#include "iengine/materials/MaterialManager.h"

#ifdef IENGINE_WEBGPU_SUPPORT
#include "iengine/renderers/webgpu/WebGPURenderer.h"
#endif

#include <iostream>
#include <memory>

namespace iengine {

    Engine::Engine(const EngineOptions& options) {
        setRenderer(options.renderer, false);
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

        // 初始化渲染器（从场景获取context）
        initRenderer();

		// 预热材质管理器中的内置着色器
        ShaderLib::registerBuiltInShaders();

		// 预热材质管理器中的内置材质
		//registerBuiltInMaterials();   // 需要实现，甚至是作为 MaterialManager 的静态方法

        // 注意：不在这里启动主循环，由外部应用程序控制
        // while (running_) {
        //     tick();
        // }
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
        setActiveScene(name);
    }

    void Engine::setActiveScene(const std::string& name) {
        auto it = scenes_.find(name);
        if (it != scenes_.end()) {
            activeScene_ = it->second;
            // 设置Scene的Context类型与当前渲染器匹配
            if (activeRenderer_) {
                if (dynamic_cast<OpenGLRenderer*>(activeRenderer_.get())) {
                    activeScene_->setContextType(RendererType::OpenGL);
                }
            }
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
        setRenderer(renderer);
    }

    void Engine::setRenderer(RendererType renderer, bool init) {
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

        if (init) {
            initRenderer();
        }
    }


    void Engine::initRenderer() {
        if (!activeScene_) {
            std::cerr << "Warning: No active scene set, cannot initialize renderer" << std::endl;
            return;
        }
        
        if (activeRenderer_) {
            // 从scene获取context并传给renderer
            auto context = activeScene_->getContext();
            if (context) {
                activeRenderer_->initialize(context);
                std::cout << "Renderer initialized with context from scene" << std::endl;
            } else {
                std::cerr << "Error: Scene has no context" << std::endl;
            }
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