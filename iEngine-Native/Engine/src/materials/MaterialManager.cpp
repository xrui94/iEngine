#include "../../include/iengine/materials/MaterialManager.h"

#include <iostream>

namespace iengine {

    MaterialManager::~MaterialManager() {
        cleanup();
    }

    void MaterialManager::registerMaterial(const std::string& name, const std::shared_ptr<Material>& material) {
        materials_[name] = material;
    }

    std::shared_ptr<Material> MaterialManager::getMaterial(const std::string& name) const {
        auto it = materials_.find(name);
        if (it != materials_.end()) {
            return it->second.lock(); // 使用 weak_ptr.lock() 获取 shared_ptr
        }
        return nullptr;
    }

    void MaterialManager::unregisterMaterial(const std::string& name) {
        materials_.erase(name);
    }

    void MaterialManager::cleanup() {
        materials_.clear();
    }

    void MaterialManager::preheatShaders() {
        // 预热着色器的实现
        std::cout << "Preheating shaders..." << std::endl;
        // 在实际实现中，这里会初始化常用的着色器程序
    }

} // namespace iengine