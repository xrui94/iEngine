#include "iengine/materials/PhongMaterial.h"
#include "iengine/renderers/Context.h"
#include "iengine/renderers/opengl/OpenGLUniforms.h"
#include "iengine/views/cameras/Camera.h"
#include "iengine/core/Mesh.h"
#include "iengine/lights/Light.h"
#include "iengine/math/Matrix4.h"

#include <iostream>

namespace iengine {
    PhongMaterial::PhongMaterial(const PhongMaterialParams& params)
        : Material(params.name, params.shaderName), 
          color(params.color), 
          specular(params.specular), 
          shininess(params.shininess) {}
    
    void PhongMaterial::setColor(float r, float g, float b) {
        color.r = r;
        color.g = g;
        color.b = b;
    }
    
    void PhongMaterial::setSpecular(float r, float g, float b) {
        specular.r = r;
        specular.g = g;
        specular.b = b;
    }
    
    void PhongMaterial::setShininess(float shininess) {
        this->shininess = shininess;
    }
    
    std::map<std::string, bool> PhongMaterial::getShaderMacroDefines() const {
        std::map<std::string, bool> defines;
        defines["HAS_COLOR"] = true;
        defines["HAS_SPECULAR"] = true;
        return defines;
    }
    
    std::map<std::string, UniformValue> PhongMaterial::getUniforms(
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
        
        // 设置 Phong 材质参数
        uniforms["uColor"] = UniformValue::fromVec4(color.r, color.g, color.b, color.a);
        uniforms["uSpecular"] = UniformValue::fromVec3(specular.r, specular.g, specular.b);
        uniforms["uShininess"] = UniformValue(shininess);
        
        std::cout << "PhongMaterial::getUniforms() - Computed Phong uniforms" << std::endl;
        
        return uniforms;
    }
    
    TextureInfo PhongMaterial::getTextures() {
        TextureInfo info;
        // Phong材质没有纹理（可以在未来扩展）
        return info;
    }
    
    RenderPipelineState PhongMaterial::getRenderPipelineState() const {
        RenderPipelineState state;
        state.depthTest = depthTest;
        state.depthWrite = depthWrite;
        state.depthFunc = depthFunc;
        state.blend = transparent;
        state.cullFace = !doubleSided;
        return state;
    }
}