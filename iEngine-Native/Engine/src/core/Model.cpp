#include "../../include/iengine/core/Model.h"

namespace iengine {
    Model::Model(const std::string& name, 
                 std::shared_ptr<Mesh> mesh, 
                 std::shared_ptr<Material> material)
        : name(name), mesh(mesh), material(material) {}
    
    void Model::setPosition(float x, float y, float z) {
        transform_.setIdentity();
        // 这里应该设置平移变换
    }
    
    void Model::setRotation(float x, float y, float z) {
        // 这里应该设置旋转变换
    }
    
    void Model::setScale(float x, float y, float z) {
        // 这里应该设置缩放变换
    }
    
    void Model::addAnimation(const AnimationCallback& callback) {
        animations_.push_back(callback);
    }
    
    void Model::update(float deltaTime) {
        for (const auto& callback : animations_) {
            callback(*this, deltaTime);
        }
    }
}