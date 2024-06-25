#pragma once

// #include "GLFW/glfw3.h"

#include "RenderingContext.h"

#include "Shader.h"
#include "Model.h"
#include "Enum.h"
#include "ShaderProgram.h"

struct GLFWwindow;


struct ShaderCom
{
    uint32_t VertShader;
    uint32_t FragShader;
    uint32_t GeomShader;
};
    
class OpenGLRenderContext : public RenderingContext
{
public:
    OpenGLRenderContext::OpenGLRenderContext(GLFWwindow* window);

    bool Init() override;
    void SwapBuffers() override;
    void Destroy() override;

    static std::shared_ptr<RenderingContext> Create(void* window);


    // 获取OpenGL内置变量
    void Version();
    void RedBit();
    void GreenBit();
    void BlueBit();
    void AlphaBit();
    void GetCurrentProgram();


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
    void SetViewport(int32_t width, int32_t height);


    // 清理缓冲区
    void Clear();


    // 设置顶点属性、顶点属性的索引、实例矩阵数据
    uint32_t GetPrimitiveType(n_GIECore::MeshType MeshType);
    uint32_t GetVertexDataType(n_GIECore::VertexDataType dataType);


    // 顶点数组对象（Vertex Array Object, VAO）
    uint32_t CreateVertexArray(std::shared_ptr<n_GIECore::ShaderProgram> pShader, std::shared_ptr<n_GIECore::Model> pRenderModel);
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
    void SetVertexBuffer(std::shared_ptr<n_GIECore::ShaderProgram> pShader, std::map<std::string, n_GIECore::BufferLayout> bufferLayout);
    void SetInstanceBuffer(std::shared_ptr<n_GIECore::ShaderProgram> pShader, n_GIECore::BufferLayout instanceInfo);


    // Uniform Block数据
    uint32_t CreateUniformBlock(uint32_t index, size_t bufferSize, float* buffer);
    void SetUniformBlock(uint32_t program, std::string name, uint32_t index);

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
    uint32_t OpenGLRenderContext::CreateTexture(n_GIECore::TextureImage& textureImage);
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
    void SetUniformData(uint32_t program, std::string name, n_GIECore::UniformDataType dataType, std::vector<T> values);
    
    // template <typename T>
    // T GetUniformData(uint32_t program, std::string name);

    void ReadPixels(const uint32_t* xPos, const uint32_t* yPos, const uint32_t* xWidth, const uint32_t* yWidth, uint8_t* pixels);   


    // 绘制模型（图元）
    void Draw(std::shared_ptr<n_GIECore::Model> pRenderModel);

private:
    GLFWwindow* m_Window;

protected:
    void GetContext();

};