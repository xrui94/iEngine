#pragma once

#include "Material.h"
#include "Color.h"
#include <memory>

namespace iengine {
    // 前向声明
    class Texture;
    
    struct PbrMaterialParams {
        std::string name = "pbr";
        std::string shaderName = "base_pbr";
        
        // 基础颜色
        Color baseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
        std::shared_ptr<Texture> baseColorMap = nullptr;
        
        // 金属度和粗糙度
        float metallic = 0.0f;
        float roughness = 1.0f;
        std::shared_ptr<Texture> metallicRoughnessMap = nullptr;
        
        // 法线贴图
        std::shared_ptr<Texture> normalMap = nullptr;
        float normalScale = 1.0f;
        
        // 环境遮蔽
        std::shared_ptr<Texture> aoMap = nullptr;
        float aoStrength = 1.0f;
        
        // 自发光
        Color emissiveColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
        float emissiveIntensity = 1.0f;
        std::shared_ptr<Texture> emissiveMap = nullptr;
    };
    
    class PbrMaterial : public Material {
    public:
        // 基础颜色（albedo/baseColor）
        Color baseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
        std::shared_ptr<Texture> baseColorMap = nullptr;
        
        // 金属度和粗糙度
        float metallic = 0.0f;
        float roughness = 1.0f;
        std::shared_ptr<Texture> metallicRoughnessMap = nullptr;
        
        // 法线贴图
        std::shared_ptr<Texture> normalMap = nullptr;
        float normalScale = 1.0f;
        
        // 环境遮蔽
        std::shared_ptr<Texture> aoMap = nullptr;
        float aoStrength = 1.0f;
        
        // 自发光
        Color emissiveColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
        float emissiveIntensity = 1.0f;
        std::shared_ptr<Texture> emissiveMap = nullptr;
        
        PbrMaterial(const PbrMaterialParams& params = PbrMaterialParams{});
        virtual ~PbrMaterial() = default;
        
        // Setters - 基础属性
        void setBaseColor(float r, float g, float b);
        void setMetallic(float metallic);
        void setRoughness(float roughness);
        void setNormalScale(float scale);
        void setAoStrength(float strength);
        void setEmissiveColor(float r, float g, float b);
        void setEmissiveIntensity(float intensity);
        
        // Setters - 贴图
        void setBaseColorMap(std::shared_ptr<Texture> texture);
        void setMetallicRoughnessMap(std::shared_ptr<Texture> texture);
        void setNormalMap(std::shared_ptr<Texture> texture);
        void setAoMap(std::shared_ptr<Texture> texture);
        void setEmissiveMap(std::shared_ptr<Texture> texture);
        
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