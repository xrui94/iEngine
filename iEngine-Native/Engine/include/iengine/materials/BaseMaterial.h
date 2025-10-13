#pragma once

#include "Material.h"
#include "Color.h"
#include "../lights/Light.h"

namespace iengine {
    struct BaseMaterialParams {
        std::string name = "base";
        std::string shaderName = "base_material";
        Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    };
    
    class BaseMaterial : public Material {
    public:
        Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        
        BaseMaterial(const BaseMaterialParams& params = BaseMaterialParams{});
        
        void setColor(float r, float g, float b);
        
        // Material接口实现
        std::map<std::string, bool> getShaderMacroDefines() const override;
        std::map<std::string, class UniformValue> getUniforms(
            std::shared_ptr<Context> context,
            std::shared_ptr<Camera> camera,
            std::shared_ptr<class Model> model,  // 改为传递 Model
            const std::vector<std::shared_ptr<Light>>& lights) override;
        TextureInfo getTextures() override;
        
        // 获取渲染管线状态
        RenderPipelineState getRenderPipelineState() const;
    };
}