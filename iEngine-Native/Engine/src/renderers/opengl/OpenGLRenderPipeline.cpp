#include "iengine/renderers/opengl/OpenGLRenderPipeline.h"
#include "iengine/renderers/opengl/OpenGLContext.h"
#include "iengine/core/Mesh.h"
#include <glad/glad.h>

#include <iostream>

namespace iengine {

    OpenGLRenderPipeline::OpenGLRenderPipeline() : isBound_(false), vao_(0) {}

    OpenGLRenderPipeline::~OpenGLRenderPipeline() {
        cleanup();
    }

    void OpenGLRenderPipeline::initialize() {
        // 初始化渲染管线
        std::cout << "Initializing OpenGL Render Pipeline" << std::endl;
    }

    void OpenGLRenderPipeline::cleanup() {
        // 清理资源
        if (vao_ != 0 && context_) {
            context_->deleteVAO(vao_);
            vao_ = 0;
        }
        std::cout << "Cleaning up OpenGL Render Pipeline" << std::endl;
    }
    
    void OpenGLRenderPipeline::setupVAO(std::shared_ptr<Mesh> mesh, std::shared_ptr<OpenGLShaderProgram> shader, std::shared_ptr<OpenGLContext> context) {
        context_ = context;
        shaderProgram_ = shader;
        
        if (!mesh || !shader || !context) {
            std::cerr << "OpenGLRenderPipeline::setupVAO - Invalid parameters" << std::endl;
            return;
        }
        
        // 创建 VAO
        vao_ = context->createVAO();
        if (vao_ == 0) {
            std::cerr << "OpenGLRenderPipeline::setupVAO - Failed to create VAO" << std::endl;
            return;
        }
        
        // 绑定 VAO
        context->bindVAO(vao_);
        
        // 绑定 VBO
        if (mesh->getVBO()) {
            unsigned int vboId = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(mesh->getVBO()));
            glBindBuffer(GL_ARRAY_BUFFER, vboId);
        } else {
            std::cerr << "OpenGLRenderPipeline::setupVAO - Mesh VBO is null" << std::endl;
            context->unbindVAO();
            return;
        }
        
        // 设置顶点属性指针
        auto layout = mesh->getVertexLayout();
        unsigned int programId = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(shader->program));
        
        for (const auto& attr : layout.attributes) {
            int location = context->getAttribLocation(programId, attr.name);
            if (location >= 0) {
                context->enableVertexAttribArray(location);
                
                // 设置顶点属性指针
                unsigned int type = GL_FLOAT; // 默认为 float
                int size = 3; // 默认为 3 个组件
                
                if (attr.format == "float32x2") {
                    size = 2;
                } else if (attr.format == "float32x3") {
                    size = 3;
                } else if (attr.format == "float32x4") {
                    size = 4;
                }
                
                context->vertexAttribPointer(
                    location,
                    size,
                    type,
                    false, // normalized
                    layout.arrayStride,
                    reinterpret_cast<const void*>(attr.offset)
                );
                
                std::cout << "OpenGLRenderPipeline::setupVAO - Set attribute '" << attr.name 
                          << "' at location " << location << " with size " << size << std::endl;
            } else {
                std::cout << "OpenGLRenderPipeline::setupVAO - Attribute '" << attr.name 
                          << "' not found in shader" << std::endl;
            }
        }
        
        // 绑定 IBO（如果有）
        if (mesh->getIBO()) {
            unsigned int iboId = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(mesh->getIBO()));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
        }
        
        // 解绑 VAO
        context->unbindVAO();
        
        std::cout << "OpenGLRenderPipeline::setupVAO - VAO setup complete: " << vao_ << std::endl;
    }

    void OpenGLRenderPipeline::bind() {
        if (shaderProgram_) {
            shaderProgram_->bind();
        }
        
        if (vao_ != 0 && context_) {
            context_->bindVAO(vao_);
        }
        
        isBound_ = true;
    }

    void OpenGLRenderPipeline::unbind() {
        if (context_) {
            context_->unbindVAO();
        }
        
        if (shaderProgram_) {
            shaderProgram_->unbind();
        }
        
        isBound_ = false;
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