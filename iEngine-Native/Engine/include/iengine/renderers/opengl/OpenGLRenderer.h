#pragma once

#include "../Renderer.h"
#include <memory>
#include <map>
#include <string>

namespace iengine {
    // 前向声明
    class Camera;
    class Scene;
    class Mesh;
    class Material;
    class Context;
    class Light;
    class OpenGLContext;
    class OpenGLShaderProgram;
    class OpenGLRenderPipeline;
    
    class OpenGLRenderer : public Renderer {
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();
        
        bool initialize(std::shared_ptr<Context> context) override;
        void cleanup() override;
        void render(std::shared_ptr<Scene> scene) override;
        void resize(int width, int height) override;
        void clear() override;
        
        // 获取或创建渲染管线
        std::shared_ptr<OpenGLRenderPipeline> getOrCreatePipeline(
            std::shared_ptr<Mesh> mesh, 
            std::shared_ptr<OpenGLShaderProgram> shader);
        
    private:
        std::shared_ptr<Context> context_;
        std::shared_ptr<Camera> currentCamera_;
        
        // 着色器缓存
        std::map<std::string, std::shared_ptr<OpenGLShaderProgram>> shaders_;
        
        // 渲染管线缓存
        std::map<std::string, std::shared_ptr<OpenGLRenderPipeline>> renderPipelineCache_;
        
        // 获取或创建着色器
        std::shared_ptr<OpenGLShaderProgram> getOrCreateShader(
            const std::string& shaderName,
            const std::map<std::string, bool>& defines);
        
        // 生成渲染管线的哈希键
        std::string makePipelineKey(std::shared_ptr<Mesh> mesh, 
                                   std::shared_ptr<OpenGLShaderProgram> shader);
    };
}