#pragma once

#include "../Context.h"
#include <memory>
#include <string>

namespace iengine {
    // 前向声明
    class Mesh;
    class Renderable;
    class WindowInterface;
    
    struct OpenGLContextOptions {
        bool useOpenGL33 = false;
        bool useOpenGL41 = false;
        bool useOpenGL43 = false;
        bool useOpenGL45 = true;
    };
    
    class OpenGLContext : public Context {
    public:
        OpenGLContext(std::shared_ptr<WindowInterface> window, const OpenGLContextOptions& options = OpenGLContextOptions{});
        ~OpenGLContext();
        
        void init();
        void clear() override;
        void resize(int width, int height) override;
        
        // Buffer操作
        void* createVertexBuffer(size_t size) override;
        void* createIndexBuffer(size_t size) override;
        void deleteBuffer(void* buffer) override;
        void writeBuffer(void* buffer, const void* data, size_t size, size_t offset = 0) override;
        
        // 纹理操作
        void* createTexture(int width, int height, const void* data = nullptr) override;
        void deleteTexture(void* texture) override;
        void writeTexture(void* texture, const void* data, int width, int height) override;
        
        // 绘制操作
        void draw(std::shared_ptr<class Mesh> mesh) override;
        void draw(std::shared_ptr<Renderable> renderable);
        
        void* getDevice() const { return device_; }
        
        int getWidth() const { return width_; }
        int getHeight() const { return height_; }
        int getDisplayWidth() const { return displayWidth_; }
        int getDisplayHeight() const { return displayHeight_; }
        
        // OpenGL特有方法
        unsigned int createVAO();
        void bindVAO(unsigned int vao);
        void unbindVAO();
        void deleteVAO(unsigned int vao);
        
        // 着色器操作
        unsigned int createProgram(const std::string& vertexSource, const std::string& fragmentSource);
        void useProgram(unsigned int program);
        void deleteProgram(unsigned int program);
        
        // Uniform操作
        int getUniformLocation(unsigned int program, const std::string& name);
        void setUniform1f(int location, float value);
        void setUniform1i(int location, int value);  // 新增：用于纹理采样器
        void setUniform3f(int location, float x, float y, float z);
        void setUniform4f(int location, float x, float y, float z, float w);
        void setUniformMatrix4fv(int location, const float* value);
        
        // 纹理操作（新增）
        void activeTexture(int unit);  // 激活纹理单元
        void bindTexture(void* texture);  // 绑定纹理
        
        // 新增：动态uniform查询（参考Web版本）
        int getUniformCount(unsigned int program);
        struct UniformInfo {
            std::string name;
            unsigned int type;
            int size;
        };
        UniformInfo getActiveUniform(unsigned int program, int index);
        void setUniform(unsigned int type, int location, const void* value);
        
        // 顶点属性操作
        int getAttribLocation(unsigned int program, const std::string& name);
        void enableVertexAttribArray(unsigned int location);
        void vertexAttribPointer(unsigned int location, int size, unsigned int type, 
                                bool normalized, int stride, const void* pointer);
        
    private:
        std::shared_ptr<WindowInterface> window_;
        void* device_ = nullptr; // OpenGL上下文
        
        OpenGLContextOptions options_;
        
        int width_ = 800;
        int height_ = 600;
        int displayWidth_ = 800;
        int displayHeight_ = 600;
        
        // VAO支持
        bool vaoSupported_ = false;
        
        // 最大纹理单元数
        int maxTextureUnits_ = 0;
        
        // OpenGL版本信息
        int majorVersion_ = 0;
        int minorVersion_ = 0;
        
        // 辅助方法
        unsigned int compileShader(unsigned int type, const std::string& source);
        bool checkShaderCompile(unsigned int shader);
        bool checkProgramLink(unsigned int program);
    };
}