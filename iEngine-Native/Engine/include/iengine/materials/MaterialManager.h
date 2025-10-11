#pragma once

#include <memory>
#include <string>
#include <map>
#include <functional>

namespace iengine {
    // 前向声明
    class Material;
    enum class GraphicsAPI;
    
    class MaterialManager {
    public:
        MaterialManager() = default;
        ~MaterialManager();
        
        void registerMaterial(const std::string& name, const std::shared_ptr<Material>& material);
        std::shared_ptr<Material> getMaterial(const std::string& name) const;
        void unregisterMaterial(const std::string& name);
        void cleanup();
        void preheatShaders();
        
    private:
        std::map<std::string, std::weak_ptr<Material>> materials_;
    };
    
    // 注册内置材质
    void registerBuiltInMaterials();
}