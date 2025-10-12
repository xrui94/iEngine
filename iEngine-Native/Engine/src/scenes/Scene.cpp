#include "iengine/scenes/Scene.h"
#include "iengine/core/Model.h"
#include "iengine/renderers/opengl/OpenGLContext.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace iengine {
    Scene::Scene(std::shared_ptr<WindowInterface> window) : window_(window) {
        // 如果window为空，则不创建Context（适用于纯代码模式）
        if (!window_) {
            std::cout << "Scene created without window (headless mode)" << std::endl;
            return;
        }
        
        // 创建OpenGL Context
        openglContext_ = std::make_shared<OpenGLContext>(window_);
        openglContext_->init();
        
        // 默认使用OpenGL Context
        activeContext_ = openglContext_;
        
        std::cout << "Scene created with OpenGL context" << std::endl;
    }
    
    Scene::~Scene() {}
    
    void Scene::addComponent(std::shared_ptr<Model> component) {
        components_.push_back(component);
    }
    
    void Scene::removeComponent(std::shared_ptr<Model> component) {
        auto it = std::find(components_.begin(), components_.end(), component);
        if (it != components_.end()) {
            components_.erase(it);
        }
    }
    
    const std::vector<std::shared_ptr<Model>>& Scene::getComponents() const {
        return components_;
    }
    
    void Scene::addLight(std::shared_ptr<Light> light) {
        lights_.push_back(light);
    }
    
    void Scene::removeLight(std::shared_ptr<Light> light) {
        auto it = std::find(lights_.begin(), lights_.end(), light);
        if (it != lights_.end()) {
            lights_.erase(it);
        }
    }
    
    const std::vector<std::shared_ptr<Light>>& Scene::getLights() const {
        return lights_;
    }
    
    void Scene::update(float deltaTime) {
        for (auto& component : components_) {
            component->update(deltaTime);
        }
    }
    
    std::shared_ptr<Camera> Scene::getActiveCamera() const {
        return activeCamera_;
    }
    
    void Scene::setActiveCamera(std::shared_ptr<Camera> camera) {
        activeCamera_ = camera;
    }
    
    std::shared_ptr<Context> Scene::getContext() const {
        return activeContext_;
    }
    
    void Scene::setContextType(RendererType type) {
        switch (type) {
            case RendererType::OpenGL:
                if (!openglContext_) {
                    openglContext_ = std::make_shared<OpenGLContext>(window_);
                    openglContext_->init();
                }
                activeContext_ = openglContext_;
                std::cout << "Scene context set to OpenGL" << std::endl;
                break;
            default:
                throw std::runtime_error("Unsupported renderer type");
        }
    }
}