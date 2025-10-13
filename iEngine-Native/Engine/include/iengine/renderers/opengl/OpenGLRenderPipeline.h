#ifndef IENGINE_OPENGL_RENDER_PIPELINE_H
#define IENGINE_OPENGL_RENDER_PIPELINE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "OpenGLShaderProgram.h"
#include "OpenGLUniforms.h"

namespace iengine {
    // 前向声明
    class Mesh;
    class OpenGLContext;

    class OpenGLRenderPipeline {
    public:
        OpenGLRenderPipeline();
        ~OpenGLRenderPipeline();

        void initialize();
        void cleanup();

        void bind();
        void unbind();

        void setShaderProgram(const std::shared_ptr<OpenGLShaderProgram>& shaderProgram);
        std::shared_ptr<OpenGLShaderProgram> getShaderProgram() const;
        
        // 设置 VAO 和顶点属性
        void setupVAO(std::shared_ptr<Mesh> mesh, std::shared_ptr<OpenGLShaderProgram> shader, std::shared_ptr<OpenGLContext> context);

        void setUniform(const std::string& name, const UniformValue& value);
        void setUniforms(const std::unordered_map<std::string, UniformValue>& uniforms);

        void applyUniforms();

    private:
        std::shared_ptr<OpenGLShaderProgram> shaderProgram_;
        std::unordered_map<std::string, UniformValue> uniforms_;
        bool isBound_;
        
        // VAO 支持
        unsigned int vao_ = 0;
        std::shared_ptr<OpenGLContext> context_;
    };

} // namespace iengine

#endif // IENGINE_OPENGL_RENDER_PIPELINE_H