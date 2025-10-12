#include "iengine/materials/BaseMaterial.h"
#include "iengine/renderers/Context.h"
#include "iengine/renderers/opengl/OpenGLUniforms.h"
#include "iengine/views/cameras/Camera.h"
#include "iengine/core/Mesh.h"
#include "iengine/math/Matrix4.h"
#include "iengine/math/Matrix3.h"

#include <iostream>

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
    
    std::map<std::string, UniformValue> BaseMaterial::getUniforms(
        std::shared_ptr<Context> context,
        std::shared_ptr<Camera> camera,
        std::shared_ptr<Mesh> mesh,
        const std::vector<std::shared_ptr<Light>>& lights) {
        
        std::map<std::string, UniformValue> uniforms;
        
        // 计算模型-视图矩阵：uModelViewMatrix = viewMatrix * modelMatrix
        Matrix4 modelMatrix(mesh->transform);
        Matrix4 viewMatrix = camera->getViewMatrix();
        Matrix4 modelViewMatrix = viewMatrix;
        modelViewMatrix.multiply(modelMatrix);
        
        // 获取投影矩阵
        Matrix4 projectionMatrix = camera->getProjectionMatrix();
        
        // 设置 uniforms
        uniforms["uModelViewMatrix"] = UniformValue::fromMatrix4(modelViewMatrix);
        uniforms["uProjectionMatrix"] = UniformValue::fromMatrix4(projectionMatrix);
        uniforms["uBaseColor"] = UniformValue::fromVec3(color.r, color.g, color.b);
        
        std::cout << "BaseMaterial::getUniforms() - Computed uniforms: "
                  << "uModelViewMatrix, uProjectionMatrix, uBaseColor" << std::endl;
        
        return uniforms;
    }
    
    TextureInfo BaseMaterial::getTextures() {
        TextureInfo info;
        // 基础材质没有纹理
        return info;
    }
    
    RenderPipelineState BaseMaterial::getRenderPipelineState() const {
        RenderPipelineState state;
        state.depthTest = depthTest;
        state.depthWrite = depthWrite;
        state.depthFunc = depthFunc;
        state.blend = transparent;
        state.cullFace = !doubleSided;
        return state;
    }
}