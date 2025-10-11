#include "../../include/iengine/materials/PbrMaterial.h"

namespace iengine {

    PbrMaterial::PbrMaterial() : Material(), 
        baseColorFactor_(1.0f, 1.0f, 1.0f, 1.0f),
        metallicFactor_(1.0f),
        roughnessFactor_(1.0f),
        normalScale_(1.0f),
        occlusionStrength_(1.0f) {
        // PBR 材质特定初始化
    }

    const Color& PbrMaterial::getBaseColorFactor() const {
        return baseColorFactor_;
    }

    float PbrMaterial::getMetallicFactor() const {
        return metallicFactor_;
    }

    float PbrMaterial::getRoughnessFactor() const {
        return roughnessFactor_;
    }

    float PbrMaterial::getNormalScale() const {
        return normalScale_;
    }

    float PbrMaterial::getOcclusionStrength() const {
        return occlusionStrength_;
    }

    void PbrMaterial::setBaseColorFactor(const Color& color) {
        baseColorFactor_ = color;
    }

    void PbrMaterial::setMetallicFactor(float metallic) {
        metallicFactor_ = metallic;
    }

    void PbrMaterial::setRoughnessFactor(float roughness) {
        roughnessFactor_ = roughness;
    }

    void PbrMaterial::setNormalScale(float scale) {
        normalScale_ = scale;
    }

    void PbrMaterial::setOcclusionStrength(float strength) {
        occlusionStrength_ = strength;
    }

    std::vector<std::string> PbrMaterial::getShaderDefines() const {
        std::vector<std::string> defines;
        defines.push_back("MATERIAL_PBR");
        
        // 根据使用的贴图添加定义
        if (baseColorTexture_) defines.push_back("HAS_BASE_COLOR_MAP");
        if (metallicRoughnessTexture_) defines.push_back("HAS_METALLIC_ROUGHNESS_MAP");
        if (normalTexture_) defines.push_back("HAS_NORMAL_MAP");
        if (occlusionTexture_) defines.push_back("HAS_OCCLUSION_MAP");
        if (emissiveTexture_) defines.push_back("HAS_EMISSIVE_MAP");
        
        return defines;
    }

    std::unordered_map<std::string, UniformValue> PbrMaterial::getUniforms() const {
        std::unordered_map<std::string, UniformValue> uniforms;
        
        // 将Color转换为vector<float>
        std::vector<float> baseColorVec = {baseColorFactor_.getR(), baseColorFactor_.getG(), baseColorFactor_.getB(), baseColorFactor_.getA()};
        std::vector<float> emissiveVec = {emissiveFactor_.getR(), emissiveFactor_.getG(), emissiveFactor_.getB(), emissiveFactor_.getA()};
        
        uniforms["u_BaseColorFactor"] = UniformValue(baseColorVec);
        uniforms["u_MetallicFactor"] = UniformValue(metallicFactor_);
        uniforms["u_RoughnessFactor"] = UniformValue(roughnessFactor_);
        uniforms["u_NormalScale"] = UniformValue(normalScale_);
        uniforms["u_OcclusionStrength"] = UniformValue(occlusionStrength_);
        uniforms["u_EmissiveFactor"] = UniformValue(emissiveVec);
        
        return uniforms;
    }

    std::vector<std::shared_ptr<Texture>> PbrMaterial::getTextures() const {
        std::vector<std::shared_ptr<Texture>> textures;
        
        if (baseColorTexture_) textures.push_back(baseColorTexture_);
        if (metallicRoughnessTexture_) textures.push_back(metallicRoughnessTexture_);
        if (normalTexture_) textures.push_back(normalTexture_);
        if (occlusionTexture_) textures.push_back(occlusionTexture_);
        if (emissiveTexture_) textures.push_back(emissiveTexture_);
        
        return textures;
    }

    bool PbrMaterial::needsUpdate() const {
        return false; // 简单实现
    }

    void PbrMaterial::markUpdated() {
        // 简单实现
    }

} // namespace iengine