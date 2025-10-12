#include "iengine/renderers/opengl/OpenGLUniforms.h"
#include "iengine/renderers/opengl/OpenGLContext.h"
#include "iengine/math/Matrix4.h"

#include <iostream>

namespace iengine {
    // UniformValue 实现
    UniformValue::UniformValue(const std::vector<float>& value)
        : type_(Type::VEC4), float_(0.0f), vec_(value) {
        // 根据向量大小判断类型
        if (value.size() == 2) {
            type_ = Type::VEC2;
        } else if (value.size() == 3) {
            type_ = Type::VEC3;
        } else if (value.size() == 4) {
            type_ = Type::VEC4;
        } else if (value.size() == 16) {
            type_ = Type::MAT4;
        }
    }
    
    UniformValue UniformValue::fromMatrix4(const Matrix4& matrix) {
        std::vector<float> data(matrix.elements.begin(), matrix.elements.end());
        UniformValue value(data);
        return value;
    }
    
    UniformValue UniformValue::fromVec3(float x, float y, float z) {
        return UniformValue(std::vector<float>{x, y, z});
    }
    
    UniformValue UniformValue::fromVec4(float x, float y, float z, float w) {
        return UniformValue(std::vector<float>{x, y, z, w});
    }
    
    // OpenGLUniforms 实现
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