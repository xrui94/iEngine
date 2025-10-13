#include "iengine/renderers/opengl/OpenGLContext.h"
#include "iengine/windowing/Window.h"
#include "iengine/core/Mesh.h"
#include "iengine/core/Model.h"

#include <glad/glad.h>

#include <iostream>
#include <vector>
#include <stdexcept>

namespace iengine {
    OpenGLContext::OpenGLContext(std::shared_ptr<WindowInterface> window, const OpenGLContextOptions& options)
        : window_(window), options_(options) {
        if (!window_) {
            throw std::runtime_error("Invalid window interface");
        }
    }
    
    OpenGLContext::~OpenGLContext() {
        // OpenGL资源由窗口管理，这里不需要显式清理
    }
    
    void OpenGLContext::init() {
        // 通过窗口接口确保 OpenGL 上下文已创建并激活
        window_->makeContextCurrent();
        
        if (!gladLoadGL()) {
            throw std::runtime_error("Failed to initialize OpenGL context");
        }
        
        std::cout << "OpenGL版本: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "OpenGL厂商: " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "OpenGL渲染器: " << glGetString(GL_RENDERER) << std::endl;
        
        // 设置默认OpenGL状态
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        
        // 获取上下文信息
        window_->getSize(width_, height_);
        displayWidth_ = width_;
        displayHeight_ = height_;
        
        // 检查VAO支持
        vaoSupported_ = true; // 在OpenGL 3.0+中默认支持VAO
        
        // 获取最大纹理单元数
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits_);
        
        // 获取OpenGL版本
        glGetIntegerv(GL_MAJOR_VERSION, &majorVersion_);
        glGetIntegerv(GL_MINOR_VERSION, &minorVersion_);
        
        device_ = (void*)this;
        
        std::cout << "OpenGLContext初始化成功" << std::endl;
    }
    
    void OpenGLContext::clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void OpenGLContext::resize(int width, int height) {
        width_ = width;
        height_ = height;
        displayWidth_ = width;
        displayHeight_ = height;
        
        // 设置视口
        glViewport(0, 0, width, height);
        std::cout << "OpenGLContext::resize(" << width << ", " << height << ") - 视口已更新" << std::endl;
    }
    
    void* OpenGLContext::createVertexBuffer(size_t size) {
        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
        std::cout << "Created vertex buffer: " << buffer << " (size: " << size << ")" << std::endl;
        return reinterpret_cast<void*>(static_cast<uintptr_t>(buffer));
    }
    
    void* OpenGLContext::createIndexBuffer(size_t size) {
        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
        std::cout << "Created index buffer: " << buffer << " (size: " << size << ")" << std::endl;
        return reinterpret_cast<void*>(static_cast<uintptr_t>(buffer));
    }
    
    void OpenGLContext::deleteBuffer(void* buffer) {
        if (buffer) {
            GLuint bufferId = static_cast<GLuint>(reinterpret_cast<uintptr_t>(buffer));
            glDeleteBuffers(1, &bufferId);
            std::cout << "Deleted buffer: " << bufferId << std::endl;
        }
    }
    
    void OpenGLContext::writeBuffer(void* buffer, const void* data, size_t size, size_t offset) {
        GLuint bufferId = static_cast<GLuint>(reinterpret_cast<uintptr_t>(buffer));
        
        // 先绑定为顶点缓冲区尝试
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        GLenum error = glGetError();
        if (error == GL_NO_ERROR) {
            // 成功绑定为顶点缓冲区
            if (offset == 0) {
                glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
            } else {
                glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
            }
            std::cout << "Written " << size << " bytes to vertex buffer " << bufferId << std::endl;
        } else {
            // 尝试绑定为索引缓冲区
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
            if (offset == 0) {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
            } else {
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
            }
            std::cout << "Written " << size << " bytes to index buffer " << bufferId << std::endl;
        }
    }
    
    void* OpenGLContext::createTexture(int width, int height, const void* data) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        // 设置纹理参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // 上传纹理数据
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        std::cout << "Created texture: " << texture << " (" << width << "x" << height << ")" << std::endl;
        return reinterpret_cast<void*>(static_cast<uintptr_t>(texture));
    }
    
    void OpenGLContext::deleteTexture(void* texture) {
        if (texture) {
            GLuint textureId = static_cast<GLuint>(reinterpret_cast<uintptr_t>(texture));
            glDeleteTextures(1, &textureId);
            std::cout << "Deleted texture: " << textureId << std::endl;
        }
    }
    
    void OpenGLContext::writeTexture(void* texture, const void* data, int width, int height) {
        if (texture && data) {
            GLuint textureId = static_cast<GLuint>(reinterpret_cast<uintptr_t>(texture));
            glBindTexture(GL_TEXTURE_2D, textureId);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            std::cout << "Updated texture " << textureId << " with data (" << width << "x" << height << ")" << std::endl;
        }
    }
    
    void OpenGLContext::draw(std::shared_ptr<class Mesh> mesh) {
        if (!mesh || !mesh->uploaded) {
            std::cerr << "Mesh not uploaded or invalid" << std::endl;
            return;
        }
        
        // 绘制网格
        if (mesh->geometry->indexCount > 0) {
            // 使用索引绘制
            glDrawElements(
                static_cast<GLenum>(mesh->primitive->type),
                static_cast<GLsizei>(mesh->geometry->indexCount),
                GL_UNSIGNED_INT,
                0
            );
            std::cout << "Drew mesh with " << mesh->geometry->indexCount << " indices" << std::endl;
        } else {
            // 直接绘制顶点
            glDrawArrays(
                static_cast<GLenum>(mesh->primitive->type),
                0,
                static_cast<GLsizei>(mesh->geometry->vertexCount)
            );
            std::cout << "Drew mesh with " << mesh->geometry->vertexCount << " vertices" << std::endl;
        }
    }
    
    void OpenGLContext::draw(std::shared_ptr<Renderable> renderable) {
        // TODO: 绘制可渲染对象
        std::cout << "OpenGLContext::draw(Renderable) - TODO: implement with OpenGL calls" << std::endl;
    }
    
    // OpenGL特有方法实现
    unsigned int OpenGLContext::createVAO() {
        if (vaoSupported_) {
            GLuint vao;
            glGenVertexArrays(1, &vao);
            std::cout << "Created VAO: " << vao << std::endl;
            return vao;
        }
        return 0;
    }
    
    void OpenGLContext::bindVAO(unsigned int vao) {
        if (vaoSupported_) {
            glBindVertexArray(vao);
            std::cout << "Bound VAO: " << vao << std::endl;
        }
    }
    
    void OpenGLContext::unbindVAO() {
        if (vaoSupported_) {
            glBindVertexArray(0);
            std::cout << "Unbound VAO" << std::endl;
        }
    }
    
    void OpenGLContext::deleteVAO(unsigned int vao) {
        if (vaoSupported_ && vao > 0) {
            glDeleteVertexArrays(1, &vao);
            std::cout << "Deleted VAO: " << vao << std::endl;
        }
    }
    
    unsigned int OpenGLContext::createProgram(const std::string& vertexSource, const std::string& fragmentSource) {
        // 编译顶点着色器
        unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
        if (vertexShader == 0) {
            std::cerr << "Failed to compile vertex shader" << std::endl;
            return 0;
        }
        
        // 编译片段着色器
        unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
        if (fragmentShader == 0) {
            std::cerr << "Failed to compile fragment shader" << std::endl;
            glDeleteShader(vertexShader);
            return 0;
        }
        
        // 创建着色器程序
        unsigned int program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        
        // 检查链接
        if (!checkProgramLink(program)) {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(program);
            return 0;
        }
        
        // 清理着色器对象
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        std::cout << "Created shader program: " << program << std::endl;
        return program;
    }
    
    void OpenGLContext::useProgram(unsigned int program) {
        glUseProgram(program);
        std::cout << "Using shader program: " << program << std::endl;
    }
    
    void OpenGLContext::deleteProgram(unsigned int program) {
        if (program > 0) {
            glDeleteProgram(program);
            std::cout << "Deleted shader program: " << program << std::endl;
        }
    }
    
    int OpenGLContext::getUniformLocation(unsigned int program, const std::string& name) {
        int location = glGetUniformLocation(program, name.c_str());
        if (location == -1) {
            std::cout << "Warning: Uniform '" << name << "' not found in program " << program << std::endl;
        }
        return location;
    }
    
    void OpenGLContext::setUniform1f(int location, float value) {
        if (location >= 0) {
            glUniform1f(location, value);
        }
    }
    
    void OpenGLContext::setUniform3f(int location, float x, float y, float z) {
        if (location >= 0) {
            glUniform3f(location, x, y, z);
        }
    }
    
    void OpenGLContext::setUniform4f(int location, float x, float y, float z, float w) {
        if (location >= 0) {
            glUniform4f(location, x, y, z, w);
        }
    }
    
    void OpenGLContext::setUniformMatrix4fv(int location, const float* value) {
        if (location >= 0 && value) {
            glUniformMatrix4fv(location, 1, GL_FALSE, value);
        }
    }
    
    int OpenGLContext::getAttribLocation(unsigned int program, const std::string& name) {
        int location = glGetAttribLocation(program, name.c_str());
        if (location == -1) {
            std::cout << "Warning: Attribute '" << name << "' not found in program " << program << std::endl;
        }
        return location;
    }
    
    void OpenGLContext::enableVertexAttribArray(unsigned int location) {
        glEnableVertexAttribArray(location);
    }
    
    void OpenGLContext::vertexAttribPointer(unsigned int location, int size, unsigned int type, 
                                           bool normalized, int stride, const void* pointer) {
        glVertexAttribPointer(location, size, type, normalized ? GL_TRUE : GL_FALSE, stride, pointer);
    }
    
    // 辅助方法实现
    unsigned int OpenGLContext::compileShader(unsigned int type, const std::string& source) {
        unsigned int shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        
        if (!checkShaderCompile(shader)) {
            glDeleteShader(shader);
            return 0;
        }
        
        return shader;
    }
    
    bool OpenGLContext::checkShaderCompile(unsigned int shader) {
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "Shader compilation failed: " << infoLog << std::endl;
            return false;
        }
        return true;
    }
    
    bool OpenGLContext::checkProgramLink(unsigned int program) {
        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "Program linking failed: " << infoLog << std::endl;
            return false;
        }
        return true;
    }
    
    // 新增：动态uniform查询方法（参考Web版本）
    int OpenGLContext::getUniformCount(unsigned int program) {
        int count = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
        return count;
    }
    
    OpenGLContext::UniformInfo OpenGLContext::getActiveUniform(unsigned int program, int index) {
        UniformInfo info;
        const int bufferSize = 256;
        char nameBuffer[bufferSize];
        int length = 0;
        int size = 0;
        unsigned int type = 0;
        
        glGetActiveUniform(program, index, bufferSize, &length, &size, &type, nameBuffer);
        
        info.name = std::string(nameBuffer, length);
        info.type = type;
        info.size = size;
        
        return info;
    }
    
    void OpenGLContext::setUniform(unsigned int type, int location, const void* value) {
        if (location < 0) return;
        
        switch (type) {
            case GL_FLOAT:
                glUniform1f(location, *static_cast<const float*>(value));
                break;
            case GL_FLOAT_VEC2: {
                const float* vec = static_cast<const float*>(value);
                glUniform2f(location, vec[0], vec[1]);
                break;
            }
            case GL_FLOAT_VEC3: {
                const float* vec = static_cast<const float*>(value);
                glUniform3f(location, vec[0], vec[1], vec[2]);
                break;
            }
            case GL_FLOAT_VEC4: {
                const float* vec = static_cast<const float*>(value);
                glUniform4f(location, vec[0], vec[1], vec[2], vec[3]);
                break;
            }
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<const float*>(value));
                break;
            case GL_INT:
            case GL_BOOL:
                glUniform1i(location, *static_cast<const int*>(value));
                break;
            case GL_SAMPLER_2D:
            case GL_SAMPLER_CUBE:
                glUniform1i(location, *static_cast<const int*>(value));
                break;
            default:
                std::cerr << "Unsupported uniform type: " << type << std::endl;
                break;
        }
    }
}