#include "../../../include/iengine/renderers/opengl/OpenGLUniforms.h"
#include "../../../include/iengine/renderers/opengl/OpenGLContext.h"

namespace iengine {
    OpenGLUniforms::OpenGLUniforms(std::shared_ptr<OpenGLContext> context, void* program)
        : context_(context), program_(program) {
        initUniformSetters();
    }
    
    OpenGLUniforms::~OpenGLUniforms() {
        // 清理资源
    }
    
    void OpenGLUniforms::set(const std::string& name, const UniformValue& value) {
        auto it = uniformSetters_.find(name);
        if (it != uniformSetters_.end()) {
            it->second(value);
        }
    }
    
    void OpenGLUniforms::setUniforms(const std::map<std::string, UniformValue>& uniforms) {
        for (const auto& pair : uniforms) {
            set(pair.first, pair.second);
        }
    }
    
    void OpenGLUniforms::initUniformSetters() {
        // 初始化uniform设置器
        // 这里应该是实际的OpenGL uniform设置逻辑
    }
}