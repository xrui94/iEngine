#include "iengine/core/Engine.h"
#include "iengine/scenes/Scene.h"
#include "iengine/renderers/opengl/OpenGLRenderer.h"
#include "iengine/shaders/ShaderLib.h"
#include "iengine/materials/MaterialManager.h"
#include "iengine/views/cameras/PerspectiveCamera.h" // 新增：为 resize 方法中的相机类型转换

#ifdef IENGINE_WEBGPU_SUPPORT
#include "iengine/renderers/webgpu/WebGPURenderer.h"
#endif

#include <iostream>
#include <memory>
#include <chrono>

namespace iengine {

    // 实例计数器（仅用于调试警告）
    std::atomic<int> Engine::s_instanceCount{ 0 };

    Engine::Engine(const EngineOptions& options) {
        // 👇 提醒用户不要创建多个实例
        int count = ++s_instanceCount;
        if (count > 1) {
            std::cerr << "\n========================================" << std::endl;
            std::cerr << "[iengine] ⚠️  WARNING: Multiple Engine Instances!" << std::endl;
            std::cerr << "========================================" << std::endl;
            std::cerr << "Current instance count: " << count << std::endl;
            std::cerr << "\n💡 Recommended practice:" << std::endl;
            std::cerr << "   - Create only ONE Engine per application" << std::endl;
            std::cerr << "   - Use multiple Scenes to organize content" << std::endl;
            std::cerr << "   - engine.addScene(\"name\", scene)" << std::endl;
            std::cerr << "\n⚡ Impact of multiple instances:" << std::endl;
            std::cerr << "   - GPU context overhead" << std::endl;
            std::cerr << "   - Resource duplication (shaders, textures)" << std::endl;
            std::cerr << "   - Performance degradation" << std::endl;
            std::cerr << "========================================\n" << std::endl;
        }

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

        // 显式释放所有 Scene
        scenes_.clear();

        // 清理渲染器
        if (activeRenderer_) {
            activeRenderer_->cleanup();
        }

		//MaterialManager::cleanup(); // 清理材质管理器中的所有材质
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

    bool Engine::removeScene(const std::string& name) {
        return scenes_.erase(name) > 0; // erase 返回删除元素数量
    }

    bool Engine::hasScene(const std::string& name) const {
        return scenes_.find(name) != scenes_.end();
    }

    std::shared_ptr<Scene> Engine::getActiveScene() const {
        return activeScene_;
	}

    std::shared_ptr<Scene> Engine::getScene(const std::string& name) const {
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
        static auto startTime = std::chrono::steady_clock::now();
        auto currentTime = std::chrono::steady_clock::now();
        float currentTimeSeconds = std::chrono::duration<float>(currentTime - startTime).count();
        float deltaTime = currentTimeSeconds - lastTime_;
        lastTime_ = currentTimeSeconds;

        // 更新逻辑
        update(deltaTime);

        // 渲染
        render();
    }
    
    void Engine::resize(int width, int height) {
        // 参考 Web 版本的 resize 事件处理
        if (activeRenderer_) {
            activeRenderer_->resize(width, height);
            std::cout << "Engine: Resized to " << width << "x" << height << std::endl;
        }
        
        // 更新活动场景中相机的宽高比（如果是透视相机）
        if (activeScene_) {
            auto camera = activeScene_->getActiveCamera();
            if (camera) {
                // 尝试转换为透视相机并更新宽高比
                auto perspectiveCamera = std::dynamic_pointer_cast<PerspectiveCamera>(camera);
                if (perspectiveCamera && width > 0 && height > 0) {
                    perspectiveCamera->setAspect(static_cast<float>(width) / static_cast<float>(height));
                    std::cout << "Engine: Updated camera aspect ratio to " << (static_cast<float>(width) / static_cast<float>(height)) << std::endl;
                }
            }
        }
    }

    bool Engine::isReady() const noexcept {
        return activeRenderer_ != nullptr
            && activeRenderer_->isInitialized()
            && activeScene_ != nullptr;
	}

} // namespace iengine