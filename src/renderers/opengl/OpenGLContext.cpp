#include "OpenGLContext.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

// 由于CMake中的编译设置，使用：(void *)int32Var语法时，会触发如下错误：
// error C2220: 以下警告被视为错误：warning C4312: “类型强制转换”: 从“uint32_t”转换到更大的“void *”
#pragma warning(disable: 4312)  

OpenGLContext::OpenGLContext()
    : m_Window{ nullptr }
{
}

bool OpenGLContext::Init(Window* window)
{
    m_Window = window;

    // Create opengl graphics context
    // glfwMakeContextCurrent(m_Window);

    // glad: load all OpenGL function pointers，注册glad函数地址
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }

    return true;
}

void OpenGLContext::SwapBuffers()
{
    // glfwPollEvents();
    GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
    glfwSwapBuffers(glfwWindow);
}

// void OpenGLContext::Destroy()
// {
//     glfwDestroyWindow(m_Window);
//     glfwTerminate();
// }


// 激活/禁用OpenGL模式

void OpenGLContext::EnableDepthTest()
{
    glEnable(GL_DEPTH_TEST);
}

void OpenGLContext::DisableDepthTest()
{
    glDisable(GL_DEPTH_TEST);
}

void OpenGLContext::EnableCullFace()
{
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

void OpenGLContext::DisableCullFace()
{
    glDisable(GL_CULL_FACE);
}

void OpenGLContext::EnableBlend()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLContext::DisableBlend()
{
    glDisable(GL_BLEND);
}

void OpenGLContext::DepthMask(bool readOnly)
{
    glDepthMask(readOnly);
}

void OpenGLContext::EnableMultiSample()
{
    // 设置OpenGL启用多重采样
    glEnable(GL_MULTISAMPLE);
}

void OpenGLContext::DisableMultiSample()
{
    glDisable(GL_MULTISAMPLE);
}

void OpenGLContext::EnablePolygonOffset(float factor, float units)
{
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(factor, units);
}

void OpenGLContext::DisablePolygonOffset()
{
    glDisable(GL_POLYGON_OFFSET_FILL);
}

// // 设置、获取视口
// void OpenGLContext::UpdateViewport(int32_t width, int32_t height)
// {
//     m_Window->m_Width = width;
//     m_Window->m_Height = height;
// }

// void OpenGLContext::GetViewport(int32_t* width, int32_t* height)
// {
//     *width = m_Window->m_Width;
//     *height = m_Window->m_Height;
// }

void OpenGLContext::SetViewport(int32_t x, int32_t y, int32_t width, int32_t height)
{
    glViewport(x, y, width, height);
}


// 清理缓冲区
void OpenGLContext::Clear()
{
    // 渲染场景背景
    // 颜色归一化： value - minValue / (maxValue - minValue)
    // 对于颜色，最小值为0，最大值为255，因此公式简化为：normValue = value / 255
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

uint32_t OpenGLContext::GetPrimitiveType(MeshType meshType)
{
    switch (meshType)
    {
        case MeshType::MT_POINT:
            return GL_POINT;
        case MeshType::MT_LINE:
            return GL_LINES;
        case MeshType::MT_FACE:
            return GL_TRIANGLES;
        case MeshType::MT_TEXT:
            return GL_TRIANGLES;
        default:
            throw("Unknown OpenGL rendering primitive type.");
    }
}

uint32_t OpenGLContext::GetVertexDataType(VertexDataType dataType)
{
    switch (dataType)
    {
        case VertexDataType::INT32:
            return GL_INT;
        case VertexDataType::FLOAT32:
            return GL_FLOAT;
        default:
            throw("Unknown OpenGL vertex Attribute data type.");
    }
}

void OpenGLContext::BindVAO(uint32_t vao)
{
    glBindVertexArray(vao);
}

uint32_t OpenGLContext::CreateVertexArray(std::shared_ptr<ShaderProgram> pShader, std::shared_ptr<Model> pRenderModel)
{
    uint32_t vao = 99999;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    pRenderModel->SetVAO("base", &vao);

    // 创建VBO，根据顶点属性的信息,进行顶点缓冲区绑定
    uint32_t vbo = CreateVertexBuffer(pRenderModel->GetVertexAttributes());
    pRenderModel->SetVBO("base", &vbo);

    // 获取顶点属性
    std::map<std::string, BufferLayout> vertexAttrsInfo = pRenderModel->GetVertexAttrsInfo();

    // // 以绑定顶点属性数据的方式，绑定分解向量
    // let splitVectors = pRenderModel->GetSplitVectors();

    // // 高亮颜色
    // const renderStates = pRenderModel->GetRenderState();

    // let stateColors = pRenderModel->GetStateColors();
    
    // 判断实例矩阵是否存在，存在时，则进行绑定
    if (pRenderModel->HasInstances())
    {
        // 存在实例矩阵时，无需设置“componentId”属性
        SetVertexBuffer(pShader, {
            {"position", vertexAttrsInfo["Position"]},
            {"normal", vertexAttrsInfo["Normal"]},
            {"texCoord_0", vertexAttrsInfo["TexCoord_0"]}
        });

        // // 以绑定实例矩阵的方式，绑定构件ID
        // let comIDObject = CreateInstanceBuffer(pRenderModel->getComponentIDs());
        // SetInstanceBuffer(shader, vertexAttrsInfo.componentId);
        // pRenderModel->setComID('base', comIDObject);

        // 绑定顶点属性的实例矩阵
        uint32_t ibo = CreateInstanceBuffer(pRenderModel->GetInstances());
        SetInstanceBuffer(pShader, pRenderModel->GetInstanceInfo());
        pRenderModel->SetIBO("base", &ibo);

        // // 模型渲染状态Buffer
        // if (renderStates){
        //     let renderStateObj = this.createInstanceBuffer(renderStates);
        //     this.setInstanceBuffer(shader, vertexAttrsInfo.renderState);
        //     pRenderModel->setRenderStateObj('base', renderStateObj);
        // }

        // //分解向量Buffer
        // if (splitVectors){
        //     let splitVectorObj = this.createInstanceBuffer(splitVectors);
        //     this.setInstanceBuffer(shader, vertexAttrsInfo.splitVector);
        //     pRenderModel->setSplitVectorObject('base', splitVectorObj);
        // }

        // if (stateColors) {
        //     let stateColorObject = this.createInstanceBuffer(pRenderModel->getStateColors());
        //     this.setInstanceBuffer(shader, vertexAttrsInfo.stateColor);
        //     pRenderModel->setStateColorVBO('base', stateColorObject);
        // }

        // //分解向量Buffer
        // if (splitVectors){
        //     let splitVectorObj = this.createInstanceBuffer(splitVectors);
        //     this.setInstanceBuffer(shader, vertexAttrsInfo.splitVector);
        //     pRenderModel->setSplitVectorObject('base', splitVectorObj);
        // }
    }
    else
    {
        // 模型顶点属性Buffer
        SetVertexBuffer(pShader, vertexAttrsInfo);

        // // 模型渲染状态Buffer
        // if (renderStates){
        //     let renderStateObj = this.createVertexBuffer(renderStates);
        //     this.setVertexBuffer(shader, { renderState: vertexAttrsInfo.renderState });
        //     pRenderModel->setRenderStateObj('base', renderStateObj);
        // }

        // //分解向量Buffer
        // if (splitVectors){
        //     let splitVectorObj = this.createVertexBuffer(splitVectors);
        //     this.setVertexBuffer(shader, { splitVector: vertexAttrsInfo.splitVector });
        //     pRenderModel->setSplitVectorObject('base', splitVectorObj);
        // }

        // if (stateColors) {
        //     let stateColorObject = this.createVertexBuffer(pRenderModel->getStateColors());
        //     this.setVertexBuffer(shader, {stateColor: vertexAttrsInfo.stateColor });
        //     pRenderModel->setStateColorVBO('base', stateColorObject);
        // }

        // //分解向量Buffer
        // if (splitVectors){
        //     let splitVectorObj = this.createVertexBuffer(splitVectors);
        //     this.setVertexBuffer(shader, splitVectorObj, { splitVector: vertexAttrsInfo.splitVector });
        //     pRenderModel->setSplitVectorObject('base', splitVectorObj);
        // }
    }

    // 如果索引存在,则创建索引缓冲EBO/IBO(Index Buffer Object)
    if (pRenderModel->HasIndices())
    {
        uint32_t ebo = CreateIndexBuffer(pRenderModel->GetIndices());
        pRenderModel->SetEBO("base", &ebo);
    }

    // 取消当前VAO的绑定，ELEMENT_ARRAY_BUFFER与ARRAY_BUFFER不同，其解绑必须放到解绑VAO之后
    glBindVertexArray(0);
    if (pRenderModel->HasIndices())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    //
    return vao;
}


// void CreateHighlightVertexAttrsArray(std::shared_ptr<Shader> pShader, std::shared_ptr<Model> pRenderModel)
// {
//     let vao = glCreateVertexArrays();
//     glBindVertexArray(vao);
//     renderModel.setVAO('highlight', vao);

//     // 获取顶点属性数据，创建VBO，根据顶点属性的信息,进行顶点缓冲区绑定
//     let vbo = this.createVertexBuffer(renderModel.getVertexAttributes());
//     renderModel.setVBO('highlight', vbo);

//     // 获取顶点属性信息
//     let vertexAttributeInfo = renderModel.getIDVertexAttributeInfo();

//     // 模型渲染状态
//     const renderStates = renderModel.getRenderState();

//     // 判断实例矩阵是否存在，存在时，则进行绑定
//     if (renderModel.hasInstance()) {
//         // 存在实例矩阵时，无需设置“componentId”属性
//         this.setVertexBuffer(shader, {
//             position: vertexAttributeInfo.position,
//         });

//         // 以绑定实例矩阵的方式，绑定构件ID
//         let comIDObject = this.createInstanceBuffer(renderModel.getComponentIDs());
//         this.setInstanceBuffer(shader, vertexAttributeInfo.componentId);
//         renderModel.setComID('highlight', comIDObject);

//         // 绑定实例矩阵
//         let ibo = this.createInstanceBuffer(renderModel.getInstances());
//         this.setInstanceBuffer(shader, renderModel.getInstanceInfo());
//         renderModel.setIBO('highlight', ibo);

//         // 模型渲染状态Buffer
//         if (renderStates){
//             let renderStateObj = this.createInstanceBuffer(renderStates);
//             this.setInstanceBuffer(shader, vertexAttributeInfo.renderState);
//             renderModel.setRenderStateObj('highlight', renderStateObj);
//         }
//     }
//     else {
//         this.setVertexBuffer(shader, vertexAttributeInfo);

//         // 模型渲染状态Buffer
//         if (renderStates){
//             let renderStateObj = this.createVertexBuffer(renderStates);
//             this.setVertexBuffer(shader, { renderState: vertexAttributeInfo.renderState });
//             renderModel.setRenderStateObj('highlight', renderStateObj);
//         }
//     }

//     // 如果索引存在,则创建索引缓冲EBO/IBO(Index Buffer Object)
//     if (renderModel.hasIndice()) {
//         let ebo = this.createIndexBuffer(renderModel.getIndices())
//         renderModel.setEBO('highlight', ebo);
//     }

//     // 取消当前VAO的绑定
//     glBindVertexArray(null);
//     // ELEMENT_ARRAY_BUFFER与ARRAY_BUFFER不同，其解绑必须放到解绑VAO之后
//     if (renderModel.hasIndice()) {
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, null);
//     }

//     //
//     return vao;
// }

uint32_t OpenGLContext::BindVertexBuffer(uint32_t bufferType, std::vector<uint8_t>& buffer, uint32_t usage)
{
    uint32_t bufferHandle = 99999;
    glCreateBuffers(1, &bufferHandle);
    glBindBuffer(bufferType, bufferHandle);
    glBufferData(bufferType, sizeof(uint8_t) * buffer.size(), buffer.data(), usage);
    return bufferHandle;
}

uint32_t OpenGLContext::CreateVertexBuffer(std::vector<uint8_t>& vertexAttrs)
{
    return BindVertexBuffer(GL_ARRAY_BUFFER, vertexAttrs, GL_STATIC_DRAW);
}

uint32_t OpenGLContext::CreateIndexBuffer(std::vector<uint8_t>& indices)
{
    return BindVertexBuffer(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW);
}

uint32_t OpenGLContext::CreateInstanceBuffer(std::vector<uint8_t>& instances)
{
    return BindVertexBuffer(GL_ARRAY_BUFFER, instances, GL_STATIC_DRAW);
}

void OpenGLContext::UpdateInstanceBuffer(uint32_t BufferHandle, std::vector<uint8_t>& instances)
{
    glBindBuffer(GL_ARRAY_BUFFER, BufferHandle);
    // console.log(this.model.instances.byteLength, instances.byteLength, glGetBufferParameter(GL_ARRAY_BUFFER, GL_BUFFER_SIZE));
    glBufferData(GL_ARRAY_BUFFER, sizeof(uint8_t) * instances.size(), instances.data(), GL_STATIC_DRAW);   // 全部更新
    // glBufferSubData(GL_ARRAY_BUFFER, 0, instances, 0);   // 局部更新
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLContext::SetVertexBuffer(std::shared_ptr<ShaderProgram> pShader, std::map<std::string, BufferLayout> bufferLayout)
{
    // 解除绑定
    //glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);

    // 指定顶点属性数据的绑定信息
    uint32_t attrLoc = 99999;
    for(auto& attrInfo : bufferLayout)
    {
        BufferLayout attr = attrInfo.second;
        // std::cout << pShader->GetProgram() << std::endl;
        attrLoc = GetAttrLocation(pShader->GetProgram(), attr.Name);
        // std::cout << "attr.Name:\t" << attr.Name << ",\t attrLoc:\t" << attrLoc << std::endl;
        glEnableVertexAttribArray(attrLoc);
        glVertexAttribPointer(
            attrLoc,
            attr.Count,
            GetVertexDataType(VertexDataType(attr.DataType)),
            attr.Normalized,
            attr.Stride,
            (void *)attr.Offset);
    }

    // 解除绑定
    // glBindBuffer(GL_ARRAY_BUFFER, null);
}

void OpenGLContext::SetInstanceBuffer(std::shared_ptr<ShaderProgram> pShader, BufferLayout layoutInfo)
{
    uint32_t attrLoc = GetAttrLocation(pShader->GetProgram(), layoutInfo.Name);
    for (uint32_t i = attrLoc, len = attrLoc + layoutInfo.Batch; i < len; i++) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(
            i,
            layoutInfo.Count,
            GetVertexDataType(VertexDataType(layoutInfo.DataType)),
            layoutInfo.Normalized,
            layoutInfo.Stride,
            (void *)((i - attrLoc) * layoutInfo.Offset));
        //
        glVertexAttribDivisor(i, 1);
    }

    // 解除绑定
    // glBindBuffer(GL_ARRAY_BUFFER, null);
}



// Uniform Block数据

uint32_t OpenGLContext::CreateUniformBlock(uint32_t index, size_t bufferSize, float* buffer)
{
    // 创建Uniform缓冲块
    uint32_t ubo = 99999;
    glCreateBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, buffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // 绑定Uniform缓冲块，到相同的绑定点上
    glBindBufferBase(GL_UNIFORM_BUFFER, index, ubo);

    //
    return ubo;
}

void OpenGLContext::SetUniformBlock(uint32_t shaderProgram, std::string name, uint32_t index)
{
    uint32_t uniformBlockIdx = glGetUniformBlockIndex(shaderProgram, name.c_str());
    glUniformBlockBinding(shaderProgram, uniformBlockIdx, index);
}

// void OpenGLContext::UpdateUniformBlock(uint32_t bufferHandle, uint32_t dstByteOffset, size_t bufferSize, std::vector<T> buffer)
// {
//     // 传递矩阵给unifor block内存区域（GLSL）   
//     glBindBuffer(GL_UNIFORM_BUFFER, bufferHandle);
//     glBufferSubData(GL_UNIFORM_BUFFER, dstByteOffset, bufferSize, (void *)buffer.data());
//     glBindBuffer(GL_UNIFORM_BUFFER, 0);
// }

// void OpenGLContext::UpdateUniformBlock(uint32_t bufferHandle, uint32_t dstByteOffset, size_t bufferSize, T buffer)
// {
//     // 传递矩阵给unifor block内存区域（GLSL）   
//     glBindBuffer(GL_UNIFORM_BUFFER, bufferHandle);
//     glBufferSubData(GL_UNIFORM_BUFFER, dstByteOffset, bufferSize, buffer);
//     glBindBuffer(GL_UNIFORM_BUFFER, 0);
// }



// 纹理操作    

uint32_t OpenGLContext::CreateTexture(TextureImage& textureImage)
{
    // 创建并绑定纹理对象
    uint32_t textureHandle;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    // 设置像素存储格式
    // ref: https://webglfundamentals.org/webgl/lessons/zh_cn/webgl-data-textures.html
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 设置纹理图片的wrapping参数
    // 一般UV坐标或ST坐标处于0到1之间，有时也会超过1，超过1时的显示方式
    // GL_REPEAT 平铺
    // GL_MIRRORED_REPEAT 镜像（倒影）平铺
    // GL_CLAMP_TO_EDGE 使用边缘颜色填充
    // ref: https://www.jianshu.com/p/8c3c1d46a8e6
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 设置纹理图片的filtering参数
    // 图片的真实大小为256*256，实际绘制时的区域有可能大于图片实际尺寸，也有可能小于图片实际尺寸
    // 用于定义大于或小于真实尺寸时的图片算法，NEAREST效率高，LINEAR效率低但效果好，图片较小时看不出差别
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 设置纹理图片
    if (textureImage.nChannels == 1)
    {
    }
    else if (textureImage.nChannels == 2)
    {
    }
    else if (textureImage.nChannels == 3)
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            textureImage.width,
            textureImage.height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            textureImage.image);
    }
    else if (textureImage.nChannels == 4)
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            textureImage.width,
            textureImage.height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            textureImage.image);
    }
    else
    {
        std::cout << "Invalid texture image channels:\t" << textureImage.nChannels << std::endl;
        std::cerr << "Invalid texture image, its number of channels cannot be greater than 4." << std::endl;
    }
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // 设置mipmap，进行每个缩放层级的纹理数据的预计算
    // ref: http://www.360doc.com/content/17/1204/10/9008018_709720604.shtml
    glGenerateMipmap(GL_TEXTURE_2D);
    //
    return textureHandle;
}

void OpenGLContext::ActiveTexture(uint32_t uint)
{
    glActiveTexture(GL_TEXTURE0 + uint);
}

void OpenGLContext::BindTexture(uint32_t handle)
{
    glBindTexture(GL_TEXTURE_2D, handle);
}


// 获取GPU数据

void OpenGLContext::ReadPixels(const uint32_t* xPos, const uint32_t* yPos, const uint32_t* xWidth, const uint32_t* yWidth, uint8_t* pixels)
{
    glReadPixels(*xPos, *yPos, *xWidth, *yWidth, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}



// Shader操作

void OpenGLContext::CompileShader(const std::string vertCode, const std::string fragCode, const std::string geomCode, ShaderCom shaderCom)
{
    // parse file stream form JS const variable
    const char* vertCode_ = vertCode.c_str();
    uint32_t vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertCode_, nullptr);
    glCompileShader(vertShader);
    CheckCompileErrors(&vertShader, "vertex shader");
    shaderCom.VertShader = vertShader;

    const char* fragCode_ = fragCode.c_str();
    uint32_t fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragCode_, nullptr);
    glCompileShader(fragShader);
    CheckCompileErrors(&fragShader, "fragment shader");
    shaderCom.FragShader = fragShader;

    uint32_t geomShader = 0;
    if (geomCode.length() > 0) {
        const char* geomCode_ = geomCode.c_str();
        geomShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geomShader, 1, &geomCode_, nullptr);
        glCompileShader(geomShader);
        CheckCompileErrors(&geomShader, "geometry shader");
        shaderCom.GeomShader = fragShader;
    }
}

void OpenGLContext::LinkProgram(uint32_t program)
{
    glLinkProgram(program);

    //
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_LINK_ERROR of type: " << infoLog << std::endl;
    }
}

void OpenGLContext::UseProgram(uint32_t program)
{
    glUseProgram(program);
}

uint32_t OpenGLContext::MakeProgram(const std::string vertCode, const std::string fragCode)
{
    ShaderCom shaderCom{};
    CompileShader(vertCode, fragCode, "", shaderCom);
    uint32_t program = glCreateProgram();
    glAttachShader(program, shaderCom.VertShader);
    glAttachShader(program, shaderCom.FragShader);
    LinkProgram(program);
    //
    return program;
}

void OpenGLContext::DestroyProgram(uint32_t program)
{
    // 清理WebGL程序
    glUseProgram(0);
    if (program) glDeleteProgram(program);
}

// utility function for checking shader compilation/linking errors.
void OpenGLContext::CheckCompileErrors(uint32_t* shaderHandle, std::string shaderType)
{
    GLint success;
    GLchar infoLog[1024];
    if (shaderType != "PROGRAM")
    {
        glGetShaderiv(*shaderHandle, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(*shaderHandle, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
                << shaderType << "\n"
                << infoLog << "\n -------------------------------------------------------"
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(*shaderHandle, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(*shaderHandle, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: "
                << shaderType << "\n"
                << infoLog << "\n -------------------------------------------------------"
                << std::endl;
        }
    }
}

int32_t OpenGLContext::GetAttrLocation(uint32_t program, std::string name)
{
    // return glGetAttribLocation(gl.getParameter(gl.CURRENT_PROGRAM), name);
    return glGetAttribLocation(program, name.c_str());
}

int32_t OpenGLContext::GetUniformLocation(uint32_t program, std::string name)
{
    return glGetUniformLocation(program, name.c_str());
}

void OpenGLContext::Draw(std::shared_ptr<Model> pRenderModel)
{
    if (pRenderModel->HasInstances())
    {
        if (pRenderModel->HasIndices())
        {
            // std::cout << "a1" << std::endl;
            glDrawElementsInstanced(
                GetPrimitiveType(pRenderModel->GetMeshType()),
                pRenderModel->GetIndicesCount(),
                GL_UNSIGNED_SHORT,
                0,
                pRenderModel->GetInstancesCount());
        }
        else
        {
            // std::cout << "a2" << std::endl;
            glDrawArraysInstanced(
                GetPrimitiveType(pRenderModel->GetMeshType()),
                0,
                pRenderModel->GetVerticesCount(),
                pRenderModel->GetVerticesCount());
        }
    }
    else
    {
        if (pRenderModel->HasIndices())
        {
            // std::cout << "a3" << std::endl;
            glDrawElements(
                GetPrimitiveType(pRenderModel->GetMeshType()),
                pRenderModel->GetIndicesCount(),
                GL_UNSIGNED_SHORT,
                0);
        }
        else
        {
            // std::cout << "a4" << std::endl;
            glDrawArrays(
                GetPrimitiveType(pRenderModel->GetMeshType()),
                0,
                pRenderModel->GetVerticesCount());
        }
    }

    // 取消当前VAO的绑定
    // glBindVertexArray(null);
}


#pragma warning(default: 4312) // 恢复警告
