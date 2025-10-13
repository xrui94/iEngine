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
    class UniformValue;
    
    struct TextureInfo {
        std::map<std::string, std::shared_ptr<Texture>> textures;
    };
    
    // 渲染管线状态
    struct RenderPipelineState {
        bool depthTest = true;
        bool depthWrite = true;
        int depthFunc = 0x0203; // GL_LEQUAL
        bool blend = false;
        int srcBlend = 0x0302; // GL_SRC_ALPHA
        int dstBlend = 0x0303; // GL_ONE_MINUS_SRC_ALPHA
        bool cullFace = true;
        int cullMode = 0x0405; // GL_BACK
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
        virtual std::map<std::string, UniformValue> getUniforms(
            std::shared_ptr<Context> context,
            std::shared_ptr<Camera> camera,
            std::shared_ptr<class Model> model,  // 改为传递 Model 而不是 Mesh
            const std::vector<std::shared_ptr<Light>>& lights) = 0;
        virtual TextureInfo getTextures() = 0;
    };
}