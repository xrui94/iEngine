#pragma once

#include "Material.h"
#include "Color.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace iengine {
    // 前向声明
    class Texture;
    class UniformValue;
    
    struct PhongMaterialParams {
        std::string name;
        std::string shaderName = "base_phong";
        Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);  // white
        Color specular = Color(1.0f, 1.0f, 1.0f, 1.0f);  // white
        float shininess = 32.0f;
    };
    
    class PhongMaterial : public Material {
    public:
        Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);  // white
        Color specular = Color(1.0f, 1.0f, 1.0f, 1.0f);  // white
        float shininess = 32.0f;
        
        // 纹理成员变量
        std::shared_ptr<Texture> diffuseTexture_;
        std::shared_ptr<Texture> specularTexture_;
        std::shared_ptr<Texture> normalTexture_;
        std::shared_ptr<Texture> emissiveTexture_;
        
        PhongMaterial(const PhongMaterialParams& params = PhongMaterialParams{});
        ~PhongMaterial();
        
        // Getters
        const Color& getDiffuse() const;
        const Color& getSpecular() const;
        float getShininess() const;
        const Color& getEmissive() const;
        
        // Setters
        void setDiffuse(const Color& diffuse);
        void setSpecular(const Color& specular);
        void setShininess(float shininess);
        void setEmissive(const Color& emissive);
        
        virtual std::vector<std::string> getShaderDefines() const;
        virtual std::unordered_map<std::string, UniformValue> getUniforms() const;
        virtual std::vector<std::shared_ptr<Texture>> getTextures() const;
    };
}