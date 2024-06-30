#pragma once

#include "VertexAttrs.h"
#include "Constants.h"
#include "renderers/webgpu/ShaderModule.h"

#if defined(IE_WGPU_NATIVE)
    #include <webgpu/webgpu.hpp>
#elif defined(IE_DAWN_NATIVE)
    #include <webgpu/webgpu_cpp.h>
#elif defined(__EMSCRIPTEN__)
    #include <webgpu/webgpu_cpp.h>
    #include <emscripten.h>
    #include <emscripten/val.h>
#endif

#include <glm/mat4x4.hpp>

#include <map>
#include <unordered_map>

struct ResourceBindGroup
{
    std::string label;
    wgpu::BindGroupLayout layout;
    wgpu::BindGroup group;
};

struct BindGroupData
{
    BindGroupData(uint32_t groupIndex = 0, wgpu::BindGroup bindGroup = nullptr) : groupIndex(groupIndex), bindGroup(bindGroup) {}
    uint32_t groupIndex;
    wgpu::BindGroup bindGroup;
};

struct NodeUniforms {
    glm::mat4 modelMatrix;
};


class Mesh
{
public:

#if defined(IE_WGPU_EMSCRIPTEN) || defined(IE_ONLY_EMSCRIPTEN)
    Mesh(const emscripten::val& vertexBuffer, const emscripten::val& indexBuffer, const emscripten::val& vertexAttrInfoList);
#endif

    Mesh(std::vector<float> vertexBuffer, std::vector<uint32_t> indexBuffer, std::vector<VertexAttrInfo>& vertexAttrInfoList);

    Mesh(std::vector<VertexAttrs> vertexBuffer, std::vector<uint32_t> indexBuffer, std::vector<VertexAttrInfo>& vertexAttrInfoList);

    ~Mesh();

public:
    void Destroy();

public:
    MeshType GetMeshType() const { return m_MeshType; }

    std::vector<uint8_t>& GetBBox() const { return m_BBox; }
    // std::vector<float>& GetBBox() { return m_BBox; }

    bool HasIndices() const;

    bool HasInstances() const;

    uint32_t GetVertexCount() const;

    uint32_t GetIndexCount() const;

    uint32_t GetInstanceCount() const;

    bool IsPrepared() const { return m_IsPrepared; }

    // 顶点属性数据
    void SetVertexAttrsInfo(std::map<std::string, BufferLayout>& vertexAttrsInfo) { m_VertexAttrsInfo = vertexAttrsInfo; }
    std::map<std::string, BufferLayout>& GetVertexAttrsInfo();

    std::vector<uint8_t>& GetVertexAttrs() { return m_VertexAttrs;  }
    // std::vector<float>& GetVertexAttributes() { return m_VertexAttrs; }

    // 索引数据
    std::vector<uint8_t>& GetIndices() { return m_Indices; }
    // std::vector<uint16_t>& GetIndices() { return m_Indices; }
    void SetIndices(std::vector<uint8_t>& indices) { m_Indices = indices; }

    // 顶点属性实例矩阵
    void SetInstancesInfo(BufferLayout& instancesInfo) { m_InstancesInfo = instancesInfo; }
    BufferLayout& GetInstanceInfo();

    std::vector<uint8_t>& GetInstances() { return m_Instances; }
    // std::vector<float>& GetInstances() { return m_Instances; }
    void SetInstances(std::vector<uint8_t>& instances) { m_Instances = instances; }
    
    // OpenGL对象Getter和Setter
    uint32_t GetVAO(std::string handle);

    void SetVAO(std::string handle, uint32_t* bufferObj);

    void SetVBO(std::string handle, uint32_t* bufferObj);

    void SetEBO(std::string handle, uint32_t* bufferObj);

    void SetIBO(std::string handle, uint32_t* bufferObj);

    // WebGPU对象Getter和Setter
    wgpu::RenderPipeline GetPipeline() const { return m_Pipeline; }

    wgpu::Buffer GetVertexBuffer() const { return m_GPUVertexBuffer; }

    uint64_t GetVertexBufferSize() const { return m_VertexCount * sizeof(float) * 9; }

    wgpu::Buffer GetIndexBuffer() const { return m_GPUIndexBuffer; }

    uint64_t GetIndexBufferSize() const { return m_IndexCount * sizeof(uint32_t); }

    std::map<std::string, BindGroupData>& GetBindGroups() { return m_BindGroups; }

    wgpu::ShaderModule GetShader() const { return m_Shader->GetShaderModule(); }

    void UpdateBindGroup(std::vector<ResourceBindGroup>& resourceBindGroup);

    void SendToBuffer();

private:
    void CreateBuffer(std::vector<float> vertexBuffer, std::vector<uint32_t> indexBuffer, std::vector<VertexAttrInfo>& vertexAttrInfoList);

    void CreateBuffer(std::vector<VertexAttrs> vertexBuffer, std::vector<uint32_t> indexBuffer, std::vector<VertexAttrInfo>& vertexAttrInfoList);

    void BuildShader();

private:
    MeshType m_MeshType;

    // boungding box
    std::vector<uint8_t> m_BBox;

    uint32_t m_VertexCount;

    uint32_t m_IndexCount;

    uint32_t m_InstanceCount;

    std::vector<VertexAttrInfo> m_VertexAttrInfos;

    std::vector<VertexAttrs> m_VertexVec3f;

    std::vector<uint8_t> m_VertexAttrs;

    std::vector<float> m_VertexBuffer;

    std::vector<uint32_t> m_IndexBuffer;

    std::vector<uint8_t> m_Indices;

    std::vector<uint8_t> m_Instances;

    // OpenGL渲染属性数据Buffer
    std::unordered_map<std::string, uint32_t> m_VAO;  // vao
    std::unordered_map<std::string, uint32_t> m_VBO;  // 顶点数据Buffer
    std::unordered_map<std::string, uint32_t> m_EBO;  // 顶点索引数据Buffer
    std::unordered_map<std::string, uint32_t> m_IBO;  // 顶点属性实例矩阵数据Buffer

    std::unordered_map<std::string, BufferLayout> m_VertexAttrInfo;
    
    BufferLayout m_InstanceInfo;

    // WebGPU渲染属性数据Buffer
    wgpu::Buffer m_GPUVertexBuffer;

    wgpu::Buffer m_GPUIndexBuffer;

    wgpu::Buffer m_NodeUniformBuffer;

    std::map<std::string, wgpu::BindGroupLayout> m_BindGroupLayouts;

    std::map<std::string, BindGroupData> m_BindGroups;

    //wgpu::BlendState m_BlendState;

    wgpu::RenderPipeline m_Pipeline;

    // 着色器
    std::unique_ptr<iEngine::ShaderModule> m_Shader;

    bool m_IsPrepared;
};