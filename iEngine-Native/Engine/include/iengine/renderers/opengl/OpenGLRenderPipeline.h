#ifndef IENGINE_OPENGL_RENDER_PIPELINE_H
#define IENGINE_OPENGL_RENDER_PIPELINE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "OpenGLShaderProgram.h"
#include "OpenGLUniforms.h"

namespace iengine {

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

        void setUniform(const std::string& name, const UniformValue& value);
        void setUniforms(const std::unordered_map<std::string, UniformValue>& uniforms);

        void applyUniforms();

    private:
        std::shared_ptr<OpenGLShaderProgram> shaderProgram_;
        std::unordered_map<std::string, UniformValue> uniforms_;
        bool isBound_;
    };

} // namespace iengine

#endif // IENGINE_OPENGL_RENDER_PIPELINE_H