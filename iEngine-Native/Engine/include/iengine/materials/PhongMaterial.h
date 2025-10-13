#pragma once

#include "Material.h"
#include "Color.h"
#include <memory>

namespace iengine {
    // 前向声明
    class Texture;
    
    struct PhongMaterialParams {
        std::string name = "phong";
        std::string shaderName = "base_phong";
        Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);  // white
        Color specular = Color(1.0f, 1.0f, 1.0f, 1.0f);  // white
        float shininess = 32.0f;
    };
    
    class PhongMaterial : public Material {
    public:
        Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);  // diffuse color
        Color specular = Color(1.0f, 1.0f, 1.0f, 1.0f);  // specular color
        float shininess = 32.0f;
        
        PhongMaterial(const PhongMaterialParams& params = PhongMaterialParams{});
        virtual ~PhongMaterial() = default;
        
        // Setters
        void setColor(float r, float g, float b);
        void setSpecular(float r, float g, float b);
        void setShininess(float shininess);
        
        // Material接口实现
        std::map<std::string, bool> getShaderMacroDefines() const override;
        std::map<std::string, UniformValue> getUniforms(
            std::shared_ptr<Context> context,
            std::shared_ptr<Camera> camera,
            std::shared_ptr<Model> model,
            const std::vector<std::shared_ptr<Light>>& lights) override;
        TextureInfo getTextures() override;
        
        // 获取渲染管线状态
        RenderPipelineState getRenderPipelineState() const;
    };
}