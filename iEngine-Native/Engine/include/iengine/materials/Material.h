#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include "../core/Enums.h"

namespace iengine {
    // 前向声明
    class ShaderLib;
    class Texture;
    class Context;
    class Camera;
    class Mesh;
    class Light;
    
    struct TextureInfo {
        std::map<std::string, std::shared_ptr<Texture>> textures;
    };
    
    class Material {
    public:
        std::string name;
        std::string shaderName;
        std::map<std::string, bool> shaderDefines;
        
        bool depthTest = true;
        bool depthWrite = true;
        int depthFunc = 0; // OpenGL的GL_LEQUAL等常量
        bool transparent = true;
        bool doubleSided = true;
        
        Material(const std::string& name = "default", 
                 const std::string& shaderName = "basic");
        
        virtual ~Material() = default;
        
        // 声明抽象方法，要求子类必须实现
        virtual std::map<std::string, bool> getShaderMacroDefines() const = 0;
        virtual std::map<std::string, void*> getUniforms(
            std::shared_ptr<Context> context,
            std::shared_ptr<Camera> camera,
            std::shared_ptr<Mesh> mesh,
            const std::vector<std::shared_ptr<Light>>& lights) = 0;
        virtual TextureInfo getTextures() = 0;
    };
}