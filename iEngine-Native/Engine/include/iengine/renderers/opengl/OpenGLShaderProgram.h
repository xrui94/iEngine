#pragma once

#include "OpenGLUniforms.h"
#include <memory>
#include <string>
#include <map>

namespace iengine {
    // 前向声明
    class OpenGLContext;
    
    class OpenGLShaderProgram {
    public:
        void* program = nullptr;
        std::shared_ptr<OpenGLContext> context;
        std::string vertCode;
        std::string fragCode;
        std::shared_ptr<OpenGLUniforms> uniforms;
        
        OpenGLShaderProgram(std::shared_ptr<OpenGLContext> context, 
                          const std::string& vertCode, 
                          const std::string& fragCode);
        ~OpenGLShaderProgram();
        
        void use();
        void bind();
        void unbind();
        void setUniform(const std::string& name, const UniformValue& value);
        void setUniforms(const std::map<std::string, UniformValue>& uniforms);
        
    private:
        void* createProgram();
    };
}