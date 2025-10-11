#include "../../include/iengine/materials/BaseMaterial.h"

namespace iengine {
    BaseMaterial::BaseMaterial(const BaseMaterialParams& params)
        : Material(params.name, params.shaderName), color(params.color) {}
    
    void BaseMaterial::setColor(float r, float g, float b) {
        color.r = r;
        color.g = g;
        color.b = b;
    }
    
    std::map<std::string, bool> BaseMaterial::getShaderMacroDefines() const {
        std::map<std::string, bool> defines;
        // 基础材质没有特殊的宏定义
        return defines;
    }
    
    std::map<std::string, void*> BaseMaterial::getUniforms(
        std::shared_ptr<Context> context,
        std::shared_ptr<Camera> camera,
        std::shared_ptr<Mesh> mesh,
        const std::vector<std::shared_ptr<Light>>& lights) {
        std::map<std::string, void*> uniforms;
        // 这里应该返回实际的uniform数据
        // 由于是框架代码，我们返回空映射
        return uniforms;
    }
    
    TextureInfo BaseMaterial::getTextures() {
        TextureInfo info;
        // 基础材质没有纹理
        return info;
    }
}