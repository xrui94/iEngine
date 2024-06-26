#pragma once

#include "RenderingContext.h"

#include "Shader.h"
#include "Enum.h"
#include "ShaderProgram.h"
#include "TextureImage.h"
#include "Model.h"


struct GLFWwindow;



class OpenGLRenderContext : public RenderingContext
{
public:
    OpenGLRenderContext::OpenGLRenderContext(Window* window);

    // bool Init() override;
    void SwapBuffers() override;
    // void Destroy() override;


    // // 获取OpenGL内置变量
    // void Version();
    // void RedBit();
    // void GreenBit();
    // void BlueBit();
    // void AlphaBit();
    // void GetCurrentProgram();


    // 激活/禁用OpenGL模式
    void EnableDepthTest();
    void DisableDepthTest();
    void EnableCullFace();
    void DisableCullFace();
    void EnableBlend();
    void DisableBlend();
    void DepthMask(bool readOnly);
    void EnableMultiSample();
    void DisableMultiSample();
    void EnablePolygonOffset(float factor, float units);
    void DisablePolygonOffset();


    // 设置、获取视口
    // void UpdateViewport(int32_t width, int32_t height);
    // void GetViewport(int32_t* width, int32_t* height);
    void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height);


    // 清理缓冲区
    void Clear();


    // 设置顶点属性、顶点属性的索引、实例矩阵数据
    uint32_t GetPrimitiveType(MeshType MeshType);
    uint32_t GetVertexDataType(VertexDataType dataType);


    // 顶点数组对象（Vertex Array Object, VAO）
    uint32_t CreateVertexArray(std::shared_ptr<ShaderProgram> pShader, std::shared_ptr<Model> pRenderModel);
    // uint32_t CreateHighlightVertexAttrsArray(std::shared_ptr<Shader> pShader, std::shared_ptr<Model> pRenderModel);
    void BindVAO(uint32_t vao);
    void DestroyVertexArray();


    // 创建、绑定、销毁顶点属性数据缓冲区（顶点属性、实例矩阵数据，均以顶点属性方式，绑定数据并传递到GPU缓冲区
    uint32_t CreateVertexBuffer(std::vector<uint8_t>& vertexAttrs);
    uint32_t BindVertexBuffer(uint32_t bufferType, std::vector<uint8_t>& buffer, uint32_t usage);
    void DestroyVertexAttrBuffer();


    // 创建、销毁顶点属性的索引数据缓冲区
    uint32_t CreateIndexBuffer(std::vector<uint8_t>& indices);
    void DestroyIndicesBuffer();


    // 创建、更新、销毁实例矩阵数据的缓冲区
    uint32_t CreateInstanceBuffer(std::vector<uint8_t>& instances);
    void UpdateInstanceBuffer(uint32_t BufferHandle, std::vector<uint8_t>& instances);
    void DestroyInstanceBuffer();


    // 遍历绑定信息，设置绑定数据的内存访问信息
    void SetVertexBuffer(std::shared_ptr<ShaderProgram> pShader, std::map<std::string, BufferLayout> bufferLayout);
    void SetInstanceBuffer(std::shared_ptr<ShaderProgram> pShader, BufferLayout instanceInfo);


    // Uniform Block数据
    uint32_t CreateUniformBlock(uint32_t index, size_t bufferSize, float* buffer);
    void SetUniformBlock(uint32_t shaderProgram, std::string name, uint32_t index);

    // C++模板的定义是否只能放在头文件中：https://blog.csdn.net/imred/article/details/80261632
    template<typename T>
    void UpdateUniformBlock(uint32_t bufferHandle, uint32_t dstByteOffset, size_t bufferSize, std::vector<T> buffer)
    {
        // 传递矩阵给unifor block内存区域（GLSL）   
        glBindBuffer(GL_UNIFORM_BUFFER, bufferHandle);
        glBufferSubData(GL_UNIFORM_BUFFER, dstByteOffset, bufferSize, buffer.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    template<typename T>
    void UpdateUniformBlock(uint32_t bufferHandle, uint32_t dstByteOffset, size_t bufferSize, T* buffer)
    {
        // 传递矩阵给unifor block内存区域（GLSL）   
        glBindBuffer(GL_UNIFORM_BUFFER, bufferHandle);
        glBufferSubData(GL_UNIFORM_BUFFER, dstByteOffset, bufferSize, buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UpdateUniformBlock(uint32_t bufferHandle, uint32_t dstByteOffset, glm::mat4 buffer)
    {
        // 传递矩阵给unifor block内存区域（GLSL）   
        glBindBuffer(GL_UNIFORM_BUFFER, bufferHandle);
        glBufferSubData(GL_UNIFORM_BUFFER, dstByteOffset, sizeof(buffer), &buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }


    // 纹理贴图
    uint32_t OpenGLRenderContext::CreateTexture(TextureImage& textureImage);
    void ActiveTexture(uint32_t textureUint);
    void BindTexture(uint32_t textureHandle);
    void DeactiveTexture();
    void UpdateTexture();
    void DestoryTexture();


    // // 帧缓存
    // void GetBackground();
    // void CreateFrameBuffer();
    // void BindFrameBuffer();

    // 着色器程序
    void CompileShader(const std::string vertCode, const std::string fragCode, const std::string geomCode, ShaderCom shaderCom);
    void LinkProgram(uint32_t program);
    void UseProgram(uint32_t program);
    uint32_t MakeProgram(const std::string vertCode, const std::string fragCode);
    void DestroyProgram(uint32_t program);
    void CheckCompileErrors(uint32_t* shaderHandle, std::string shaderType);


    // 设置、获取GPU数据
    int32_t GetAttrLocation(uint32_t program, std::string name);
    int32_t GetUniformLocation(uint32_t program, std::string name);

    template <typename T>
    void SetUniformData(uint32_t program, std::string name, UniformDataType dataType, std::vector<T> values)
    {
        switch (dataType)
        {
            case UniformDataType::BOOL:
                // GLSL中4字节对齐, 因此GLSL中的bool比变量, 传递JS中的int值即可
                glUniform1i(GetUniformLocation(program, name), (int32_t)values.data());
                break;
            case UniformDataType::INT:
                glUniform1i(GetUniformLocation(program, name), (int32_t)values.data());
                break;
            case UniformDataType::FLOAT:
                glUniform1f(GetUniformLocation(program, name), *(float *)values.data());
                break;
            case UniformDataType::VEC2:
                glUniform2fv(GetUniformLocation(program, name), values.size(), values.data());
                break;
            case UniformDataType::VEC3:
                glUniform3fv(GetUniformLocation(program, name), values.size(), values.data());
                break;
            case UniformDataType::VEC4:
                glUniform4fv(GetUniformLocation(program, name), values.size(), values.data());
                break;
            case UniformDataType::MAT2:
                glUniformMatrix2fv(GetUniformLocation(program, name), false, values.data());
                break;
            case UniformDataType::MAT3:
                glUniformMatrix3fv(GetUniformLocation(program, name), false, values.data());
                break;
            case UniformDataType::MAT4:
                glUniformMatrix4fv(GetUniformLocation(program, name), false, values.data());
                break;
            default:
                break;
        }
    }
    
    // template <typename T>
    // T GetUniformData(uint32_t program, std::string name);

    void ReadPixels(const uint32_t* xPos, const uint32_t* yPos, const uint32_t* xWidth, const uint32_t* yWidth, uint8_t* pixels);   


    // 绘制模型（图元）
    void Draw(std::shared_ptr<Model> pRenderModel);

private:
    Window* m_Window;

protected:
    void GetContext();

};