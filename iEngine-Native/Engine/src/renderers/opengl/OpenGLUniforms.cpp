#include "iengine/renderers/opengl/OpenGLUniforms.h"
#include "iengine/renderers/opengl/OpenGLContext.h"
#include "iengine/math/Matrix4.h"
#include "iengine/textures/Texture.h"

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
        // 严格参考Web版本的动态uniform设置机制
        unsigned int programId = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(program_));
        int uniformCount = context_->getUniformCount(programId);
        
        std::cout << "OpenGLUniforms: Found " << uniformCount << " uniforms in program " << programId << std::endl;
        
        for (int i = 0; i < uniformCount; ++i) {
            auto info = context_->getActiveUniform(programId, i);
            if (info.name.empty()) continue;
            
            // 处理数组uniform名称（移除[0]后缀），完全对齐Web版本逻辑
            std::string uniformName = info.name;
            size_t arrayPos = uniformName.find("[0]");
            if (arrayPos != std::string::npos) {
                uniformName = uniformName.substr(0, arrayPos);
            }
            
            int location = context_->getUniformLocation(programId, uniformName);
            if (location < 0) continue;
            
            // 为每个uniform创建setter，严格对应Web版本的 setter = (v: any) => this.context.setUniform(info.type, loc, v)
            uniformSetters_[uniformName] = [this, info, location](const UniformValue& value) {
                setUniformByType(info.type, location, value);
            };
            
            std::cout << "  - Registered uniform: " << uniformName 
                      << " (type: " << info.type << ", location: " << location << ")" << std::endl;
        }
        
        std::cout << "OpenGLUniforms: Initialized " << uniformSetters_.size() 
                  << " uniform setters for program " << programId << std::endl;
    }
    
    void OpenGLUniforms::setUniformByType(unsigned int type, int location, const UniformValue& value) {
        // 注意：type参数暂未使用，在Web版本中type用于直接调用OpenGL函数
        // 这里我们基于UniformValue的类型来设置uniform
        (void)type; // 避免未使用参数警告
        
        switch (value.getType()) {
            case UniformValue::Type::FLOAT:
                context_->setUniform1f(location, value.asFloat());
                break;
            case UniformValue::Type::VEC3: {
                const auto& vec = value.asVec();
                if (vec.size() >= 3) {
                    context_->setUniform3f(location, vec[0], vec[1], vec[2]);
                }
                break;
            }
            case UniformValue::Type::VEC4: {
                const auto& vec = value.asVec();
                if (vec.size() >= 4) {
                    context_->setUniform4f(location, vec[0], vec[1], vec[2], vec[3]);
                } else if (vec.size() == 3) {
                    // 如果只有3个分量，自动补充alpha为1.0
                    context_->setUniform4f(location, vec[0], vec[1], vec[2], 1.0f);
                }
                break;
            }
            case UniformValue::Type::MAT4: {
                const auto& vec = value.asVec();
                if (vec.size() >= 16) {
                    context_->setUniformMatrix4fv(location, vec.data());
                }
                break;
            }
            case UniformValue::Type::TEXTURE: {
                // 处理纹理uniform（参照Web版本）
                auto texture = value.asTexture();
                if (texture) {
                    // 1. 上传纹理到GPU（如果还没有上传）
                    if (texture->needsUpdate()) {
                        texture->upload(context_);
                    }
                    
                    // 2. 获取纹理单元（由Texture对象维护）
                    int textureUnit = texture->getUnit();
                    
                    // 3. 激活纹理单元并绑定纹理
                    context_->activeTexture(textureUnit);
                    context_->bindTexture(texture->getGpuTexture());
                    
                    // 4. 设置uniform采样器的值为纹理单元索引
                    context_->setUniform1i(location, textureUnit);
                    
                    std::cout << "OpenGLUniforms: Bound texture '" << texture->getName() 
                              << "' to unit " << textureUnit << " for uniform location " << location << std::endl;
                }
                break;
            }
            default:
                std::cerr << "OpenGLUniforms: Unsupported uniform value type: " 
                          << static_cast<int>(value.getType()) << std::endl;
                break;
        }
    }
}