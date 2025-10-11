#include "iengine/renderers/opengl/OpenGLRenderPipeline.h"

#include <iostream>

namespace iengine {

    OpenGLRenderPipeline::OpenGLRenderPipeline() : isBound_(false) {}

    OpenGLRenderPipeline::~OpenGLRenderPipeline() {
        cleanup();
    }

    void OpenGLRenderPipeline::initialize() {
        // 初始化渲染管线
        std::cout << "Initializing OpenGL Render Pipeline" << std::endl;
    }

    void OpenGLRenderPipeline::cleanup() {
        // 清理资源
        std::cout << "Cleaning up OpenGL Render Pipeline" << std::endl;
    }

    void OpenGLRenderPipeline::bind() {
        if (shaderProgram_) {
            shaderProgram_->bind();
            isBound_ = true;
        }
    }

    void OpenGLRenderPipeline::unbind() {
        if (shaderProgram_) {
            shaderProgram_->unbind();
            isBound_ = false;
        }
    }

    void OpenGLRenderPipeline::setShaderProgram(const std::shared_ptr<OpenGLShaderProgram>& shaderProgram) {
        shaderProgram_ = shaderProgram;
    }

    std::shared_ptr<OpenGLShaderProgram> OpenGLRenderPipeline::getShaderProgram() const {
        return shaderProgram_;
    }

    void OpenGLRenderPipeline::setUniform(const std::string& name, const UniformValue& value) {
        uniforms_[name] = value;
    }

    void OpenGLRenderPipeline::setUniforms(const std::unordered_map<std::string, UniformValue>& uniforms) {
        for (const auto& pair : uniforms) {
            uniforms_[pair.first] = pair.second;
        }
    }

    void OpenGLRenderPipeline::applyUniforms() {
        if (!isBound_ || !shaderProgram_) {
            return;
        }

        for (const auto& pair : uniforms_) {
            shaderProgram_->setUniform(pair.first, pair.second);
        }
    }

} // namespace iengine