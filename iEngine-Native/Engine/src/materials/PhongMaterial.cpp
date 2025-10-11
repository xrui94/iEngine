#include "../../include/iengine/materials/PhongMaterial.h"
#include "../../include/iengine/renderers/opengl/OpenGLUniforms.h"
#include <vector>

namespace iengine {

    PhongMaterial::PhongMaterial(const PhongMaterialParams& params) : Material(params.name, params.shaderName),
        color(params.color),
        specular(params.specular),
        shininess(params.shininess) {
        // Phong 材质特定初始化
    }

    PhongMaterial::~PhongMaterial() {}

    const Color& PhongMaterial::getDiffuse() const {
        return color;
    }

    const Color& PhongMaterial::getSpecular() const {
        return specular;
    }

    float PhongMaterial::getShininess() const {
        return shininess;
    }

    const Color& PhongMaterial::getEmissive() const {
        // 在PhongMaterial中没有emissive属性，返回默认的黑色
        static Color black(0.0f, 0.0f, 0.0f, 1.0f);
        return black;
    }

    void PhongMaterial::setDiffuse(const Color& diffuse) {
        color = diffuse;
    }

    void PhongMaterial::setSpecular(const Color& specular) {
        this->specular = specular;
    }

    void PhongMaterial::setShininess(float shininess) {
        this->shininess = shininess;
    }

    void PhongMaterial::setEmissive(const Color& emissive) {
        // 在PhongMaterial中没有emissive属性，忽略设置
    }

    std::vector<std::string> PhongMaterial::getShaderDefines() const {
        std::vector<std::string> defines;
        defines.push_back("MATERIAL_PHONG");
        
        // 根据使用的贴图添加定义
        if (diffuseTexture_) defines.push_back("HAS_DIFFUSE_MAP");
        if (specularTexture_) defines.push_back("HAS_SPECULAR_MAP");
        if (normalTexture_) defines.push_back("HAS_NORMAL_MAP");
        if (emissiveTexture_) defines.push_back("HAS_EMISSIVE_MAP");
        
        return defines;
    }

    std::unordered_map<std::string, UniformValue> PhongMaterial::getUniforms() const {
        std::unordered_map<std::string, UniformValue> uniforms;
        
        // 将Color转换为vector<float>
        std::vector<float> diffuseVec = {color.r, color.g, color.b, color.a};
        std::vector<float> specularVec = {specular.r, specular.g, specular.b, specular.a};
        
        uniforms["u_Diffuse"] = UniformValue(diffuseVec);
        uniforms["u_Specular"] = UniformValue(specularVec);
        uniforms["u_Shininess"] = UniformValue(shininess);
        
        return uniforms;
    }

    std::vector<std::shared_ptr<Texture>> PhongMaterial::getTextures() const {
        std::vector<std::shared_ptr<Texture>> textures;
        
        if (diffuseTexture_) textures.push_back(diffuseTexture_);
        if (specularTexture_) textures.push_back(specularTexture_);
        if (normalTexture_) textures.push_back(normalTexture_);
        if (emissiveTexture_) textures.push_back(emissiveTexture_);
        
        return textures;
    }

} // namespace iengine