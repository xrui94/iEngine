#pragma once

#include "Material.h"
#include "Color.h"
#include "../renderers/opengl/OpenGLUniforms.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace iengine {
    // 前向声明
    class Texture;
    class Context;
    class Camera;
    class Mesh;
    class Light;
    
    struct PbrMaterialParams {
        std::string name;
        std::string shaderName = "base_pbr";
        Color baseColor = Color::WHITE;
        std::shared_ptr<Texture> baseColorMap;
        // 金属光泽度粗糙度
        float metallic = 0.0f;
        float roughness = 1.0f;
        std::shared_ptr<Texture> metallicRoughnessMap;
        // 法线贴图
        std::shared_ptr<Texture> normalMap;
        float normalScale = 1.0f;
        // 环境遮蔽贴图
        std::shared_ptr<Texture> aoMap;
        float aoStrength = 1.0f;
        // 自发光
        Color emissiveColor = Color::WHITE;
        float emissiveIntensity = 1.0f;
        std::shared_ptr<Texture> emissiveMap;
    };
    
    class PbrMaterial : public Material {
    public:
        // 基础颜色（albedo/baseColor）
        Color baseColorFactor_;
        std::shared_ptr<Texture> baseColorTexture_;
        
        float metallicFactor_;
        float roughnessFactor_;
        std::shared_ptr<Texture> metallicRoughnessTexture_;
        
        std::shared_ptr<Texture> normalTexture_;
        float normalScale_;
        
        std::shared_ptr<Texture> occlusionTexture_;
        float occlusionStrength_;
        
        Color emissiveFactor_;
        std::shared_ptr<Texture> emissiveTexture_;
        
        bool needsUpdate() const;
        void markUpdated();
        
        PbrMaterial();
        virtual ~PbrMaterial() = default;
        
        const Color& getBaseColorFactor() const;
        float getMetallicFactor() const;
        float getRoughnessFactor() const;
        float getNormalScale() const;
        float getOcclusionStrength() const;
        
        void setBaseColorFactor(const Color& color);
        void setMetallicFactor(float metallic);
        void setRoughnessFactor(float roughness);
        void setNormalScale(float scale);
        void setOcclusionStrength(float strength);
        
        std::vector<std::string> getShaderDefines() const;
        std::unordered_map<std::string, UniformValue> getUniforms() const;
        std::vector<std::shared_ptr<Texture>> getTextures() const;
    };
}