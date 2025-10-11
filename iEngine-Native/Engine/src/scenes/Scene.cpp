#include "../../include/iengine/scenes/Scene.h"
#include "../../include/iengine/core/Model.h"
#include <algorithm>

namespace iengine {
    Scene::Scene() {}
    
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
}