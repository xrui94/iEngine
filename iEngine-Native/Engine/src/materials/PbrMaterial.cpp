#include "iengine/materials/PbrMaterial.h"
#include "iengine/renderers/Context.h"
#include "iengine/renderers/opengl/OpenGLUniforms.h"
#include "iengine/views/cameras/Camera.h"
#include "iengine/core/Mesh.h"
#include "iengine/lights/Light.h"
#include "iengine/textures/Texture.h"
#include "iengine/math/Matrix4.h"

#include <iostream>

namespace iengine {
    PbrMaterial::PbrMaterial(const PbrMaterialParams& params)
        : Material(params.name, params.shaderName),
          baseColor(params.baseColor),
          baseColorMap(params.baseColorMap),
          metallic(params.metallic),
          roughness(params.roughness),
          metallicRoughnessMap(params.metallicRoughnessMap),
          normalMap(params.normalMap),
          normalScale(params.normalScale),
          aoMap(params.aoMap),
          aoStrength(params.aoStrength),
          emissiveColor(params.emissiveColor),
          emissiveIntensity(params.emissiveIntensity),
          emissiveMap(params.emissiveMap) {}
    
    void PbrMaterial::setBaseColor(float r, float g, float b) {
        baseColor.r = r;
        baseColor.g = g;
        baseColor.b = b;
    }
    
    void PbrMaterial::setMetallic(float metallic) {
        this->metallic = metallic;
    }
    
    void PbrMaterial::setRoughness(float roughness) {
        this->roughness = roughness;
    }
    
    void PbrMaterial::setNormalScale(float scale) {
        this->normalScale = scale;
    }
    
    void PbrMaterial::setAoStrength(float strength) {
        this->aoStrength = strength;
    }
    
    void PbrMaterial::setEmissiveColor(float r, float g, float b) {
        emissiveColor.r = r;
        emissiveColor.g = g;
        emissiveColor.b = b;
    }
    
    void PbrMaterial::setEmissiveIntensity(float intensity) {
        this->emissiveIntensity = intensity;
    }
    
    // 贴图 setters
    void PbrMaterial::setBaseColorMap(std::shared_ptr<Texture> texture) {
        this->baseColorMap = texture;
    }
    
    void PbrMaterial::setMetallicRoughnessMap(std::shared_ptr<Texture> texture) {
        this->metallicRoughnessMap = texture;
    }
    
    void PbrMaterial::setNormalMap(std::shared_ptr<Texture> texture) {
        this->normalMap = texture;
    }
    
    void PbrMaterial::setAoMap(std::shared_ptr<Texture> texture) {
        this->aoMap = texture;
    }
    
    void PbrMaterial::setEmissiveMap(std::shared_ptr<Texture> texture) {
        this->emissiveMap = texture;
    }
    
    std::map<std::string, bool> PbrMaterial::getShaderMacroDefines() const {
        std::map<std::string, bool> defines;
        // 根据贴图是否存在设置宏定义，与TS版本保持一致
        if (baseColorMap) defines["HAS_BASECOLORMAP"] = true;
        if (metallicRoughnessMap) defines["HAS_METALLICROUGHNESSMAP"] = true;
        if (normalMap) defines["HAS_NORMALMAP"] = true;
        if (aoMap) defines["HAS_AOMAP"] = true;
        if (emissiveMap) defines["HAS_EMISSIVEMAP"] = true;
        return defines;
    }
    
    std::map<std::string, UniformValue> PbrMaterial::getUniforms(
        std::shared_ptr<Context> context,
        std::shared_ptr<Camera> camera,
        std::shared_ptr<Mesh> mesh,
        const std::vector<std::shared_ptr<Light>>& lights) {
        
        std::map<std::string, UniformValue> uniforms;
        
        // 计算模型-视图矩阵
        Matrix4 modelMatrix(mesh->transform);
        Matrix4 viewMatrix = camera->getViewMatrix();
        Matrix4 modelViewMatrix = viewMatrix;
        modelViewMatrix.multiply(modelMatrix);
        
        // 获取投影矩阵
        Matrix4 projectionMatrix = camera->getProjectionMatrix();
        
        // 设置基本 uniforms
        uniforms["uModelViewMatrix"] = UniformValue::fromMatrix4(modelViewMatrix);
        uniforms["uProjectionMatrix"] = UniformValue::fromMatrix4(projectionMatrix);
        
        // 设置 PBR 参数
        uniforms["uBaseColor"] = UniformValue::fromVec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
        uniforms["uMetallic"] = UniformValue(metallic);
        uniforms["uRoughness"] = UniformValue(roughness);
        uniforms["uNormalScale"] = UniformValue(normalScale);
        uniforms["uAoStrength"] = UniformValue(aoStrength);
        uniforms["uEmissiveColor"] = UniformValue::fromVec3(emissiveColor.r, emissiveColor.g, emissiveColor.b);
        uniforms["uEmissiveIntensity"] = UniformValue(emissiveIntensity);
        
        std::cout << "PbrMaterial::getUniforms() - Computed PBR uniforms" << std::endl;
        
        return uniforms;
    }
    
    TextureInfo PbrMaterial::getTextures() {
        TextureInfo info;
        // 返回所有贴图，与TS版本保持一致
        if (baseColorMap) {
            info.textures["uBaseColorMap"] = baseColorMap;
        }
        if (metallicRoughnessMap) {
            info.textures["uMetallicRoughnessMap"] = metallicRoughnessMap;
        }
        if (normalMap) {
            info.textures["uNormalMap"] = normalMap;
        }
        if (aoMap) {
            info.textures["uAoMap"] = aoMap;
        }
        if (emissiveMap) {
            info.textures["uEmissiveMap"] = emissiveMap;
        }
        return info;
    }
    
    RenderPipelineState PbrMaterial::getRenderPipelineState() const {
        RenderPipelineState state;
        state.depthTest = depthTest;
        state.depthWrite = depthWrite;
        state.depthFunc = depthFunc;
        state.blend = transparent;
        state.cullFace = !doubleSided;
        return state;
    }
}