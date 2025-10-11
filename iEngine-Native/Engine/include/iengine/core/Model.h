#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "Mesh.h"
#include "../materials/Material.h"
#include "../math/Matrix4.h"

namespace iengine {
    class Model {
    public:
        std::string name;
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        
        Model(const std::string& name, 
              std::shared_ptr<Mesh> mesh, 
              std::shared_ptr<Material> material);
        
        // 变换操作
        void setPosition(float x, float y, float z);
        void setRotation(float x, float y, float z);
        void setScale(float x, float y, float z);
        
        // 动画支持
        using AnimationCallback = std::function<void(Model&, float)>;
        void addAnimation(const AnimationCallback& callback);
        void update(float deltaTime);
        
    private:
        Matrix4 transform_;
        std::vector<AnimationCallback> animations_;
    };
}