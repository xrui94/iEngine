#include "iengine/renderers/opengl/OpenGLShaderProgram.h"
#include "iengine/renderers/opengl/OpenGLContext.h"
#include "iengine/renderers/opengl/OpenGLUniforms.h"

#include <iostream>

namespace iengine {
    OpenGLShaderProgram::OpenGLShaderProgram(std::shared_ptr<OpenGLContext> context, 
                                         const std::string& vertCode, 
                                         const std::string& fragCode)
        : context(context), vertCode(vertCode), fragCode(fragCode) {
        program = createProgram();
        if (program) {
            uniforms = std::make_shared<OpenGLUniforms>(context, program);
            // 重要：初始化uniform设置器，这一步不能省略！
            uniforms->initUniformSetters();
        }
    }
    
    OpenGLShaderProgram::~OpenGLShaderProgram() {
        // 清理资源
    }
    
    void* OpenGLShaderProgram::createProgram() {
        // 使用 OpenGLContext 创建着色器程序
        if (!context) {
            std::cerr << "OpenGLShaderProgram: No context set" << std::endl;
            return nullptr;
        }
        
        std::cout << "OpenGLShaderProgram: Creating shader program..." << std::endl;
        std::cout << "Vertex shader code length: " << vertCode.length() << std::endl;
        std::cout << "Fragment shader code length: " << fragCode.length() << std::endl;
        
        unsigned int programId = context->createProgram(vertCode, fragCode);
        if (programId == 0) {
            std::cerr << "OpenGLShaderProgram: Failed to create shader program" << std::endl;
            std::cerr << "Vertex shader:\n" << vertCode << std::endl;
            std::cerr << "Fragment shader:\n" << fragCode << std::endl;
            return nullptr;
        }
        
        std::cout << "OpenGLShaderProgram: Successfully created program ID: " << programId << std::endl;
        return reinterpret_cast<void*>(static_cast<uintptr_t>(programId));
    }
    
    void OpenGLShaderProgram::use() {
        // 使用着色器程序
        if (context && program) {
            unsigned int programId = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(program));
            context->useProgram(programId);
        }
    }
    
    void OpenGLShaderProgram::bind() {
        use();
    }
    
    void OpenGLShaderProgram::unbind() {
        // 取消绑定着色器程序
        if (context) {
            context->useProgram(0);
        }
    }
    
    void OpenGLShaderProgram::setUniform(const std::string& name, const UniformValue& value) {
        if (uniforms) {
            uniforms->set(name, value);
        }
    }
    
    void OpenGLShaderProgram::setUniforms(const std::map<std::string, UniformValue>& uniforms) {
        if (this->uniforms) {
            this->uniforms->setUniforms(uniforms);
        }
    }
}