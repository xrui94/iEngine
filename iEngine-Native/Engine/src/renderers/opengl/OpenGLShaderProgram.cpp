#include "../../../include/iengine/renderers/opengl/OpenGLShaderProgram.h"
#include "../../../include/iengine/renderers/opengl/OpenGLContext.h"
#include "../../../include/iengine/renderers/opengl/OpenGLUniforms.h"

namespace iengine {
    OpenGLShaderProgram::OpenGLShaderProgram(std::shared_ptr<OpenGLContext> context, 
                                         const std::string& vertCode, 
                                         const std::string& fragCode)
        : context(context), vertCode(vertCode), fragCode(fragCode) {
        program = createProgram();
        if (program) {
            uniforms = std::make_shared<OpenGLUniforms>(context, program);
        }
    }
    
    OpenGLShaderProgram::~OpenGLShaderProgram() {
        // 清理资源
    }
    
    void* OpenGLShaderProgram::createProgram() {
        // 创建OpenGL着色器程序
        // 这里应该是实际的OpenGL着色器编译和链接逻辑
        return nullptr;
    }
    
    void OpenGLShaderProgram::use() {
        // 使用着色器程序
        // 这里应该是实际的OpenGL glUseProgram 调用
    }
    
    void OpenGLShaderProgram::bind() {
        use();
    }
    
    void OpenGLShaderProgram::unbind() {
        // 取消绑定着色器程序
        // 这里应该是实际的OpenGL glUseProgram(0) 调用
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