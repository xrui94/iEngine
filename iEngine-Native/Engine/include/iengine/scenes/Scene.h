#pragma once

#include <memory>
#include <vector>

// 前向声明
namespace iengine {
    class Camera;
    class Light;
    class Model;
    
    class Scene {
    public:
        Scene();
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
        
    private:
        std::vector<std::shared_ptr<Model>> components_;
        std::vector<std::shared_ptr<Light>> lights_;
        std::shared_ptr<Camera> activeCamera_;
    };
}