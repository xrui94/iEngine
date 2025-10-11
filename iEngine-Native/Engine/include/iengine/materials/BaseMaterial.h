#pragma once

#include "Material.h"
#include "../lights/Light.h"

namespace iengine {
    struct BaseMaterialParams {
        std::string name;
        std::string shaderName;
        Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    };
    
    class BaseMaterial : public Material {
    public:
        Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        
        BaseMaterial(const BaseMaterialParams& params = BaseMaterialParams{});
        
        void setColor(float r, float g, float b);
        
        std::map<std::string, bool> getShaderMacroDefines() const override;
        std::map<std::string, void*> getUniforms(
            std::shared_ptr<Context> context,
            std::shared_ptr<Camera> camera,
            std::shared_ptr<Mesh> mesh,
            const std::vector<std::shared_ptr<Light>>& lights) override;
        TextureInfo getTextures() override;
    };
}