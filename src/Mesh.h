#pragma once

#include "VertexAttrs.h"
#include "Constants.h"
#include "renderers/webgpu/ShaderModule.h"

#if defined(IE_WGPU_NATIVE)
    #include <webgpu/webgpu.hpp>
#elif defined(IE_WGPU_EMSCRIPTEN)
    #include <webgpu/webgpu.hpp>

    #include <emscripten.h>
    #include <emscripten/val.h>
#elif defined(IE_ONLY_EMSCRIPTEN)
    #include <webgpu/webgpu_cpp.h>

    #include <emscripten.h>
    #include <emscripten/val.h>
#endif

#include <glm/mat4x4.hpp>

#include <map>

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

    uint32_t GetVertexCount() const { return m_VertexCount; }

    wgpu::RenderPipeline GetPipeline() const { return m_Pipeline; }

    wgpu::Buffer GetVertexBuffer() const { return m_GPUVertexBuffer; }

    uint64_t GetVertexBufferSize() const { return m_VertexCount * sizeof(float) * 9; }

    wgpu::Buffer GetIndexBuffer() const { return m_GPUIndexBuffer; }

    uint64_t GetIndexBufferSize() const { return m_IndexCount * sizeof(uint32_t); }

    std::map<std::string, BindGroupData>& GetBindGroups() { return m_BindGroups; }

    uint32_t GetIndexCount() const { return m_IndexCount; }

    wgpu::ShaderModule GetShader() const { return m_Shader->GetShaderModule(); }

    bool IsPrepared() const { return m_IsPrepared; }

    void UpdateBindGroup(std::vector<ResourceBindGroup>& resourceBindGroup);

    void SendToBuffer();

private:
    void CreateBuffer(std::vector<float> vertexBuffer, std::vector<uint32_t> indexBuffer, std::vector<VertexAttrInfo>& vertexAttrInfoList);

    void CreateBuffer(std::vector<VertexAttrs> vertexBuffer, std::vector<uint32_t> indexBuffer, std::vector<VertexAttrInfo>& vertexAttrInfoList);

    void BuildShader();

private:
    std::vector<VertexAttrInfo> m_VertexAttrInfos;

    uint32_t m_VertexCount;

    uint32_t m_IndexCount;

    std::vector<VertexAttrs> m_VertexVec3f;

    std::vector<float> m_VertexBuffer;

    std::vector<uint32_t> m_IndexBuffer;

    //
    wgpu::Buffer m_GPUVertexBuffer;

    wgpu::Buffer m_GPUIndexBuffer;

    wgpu::Buffer m_NodeUniformBuffer;

    std::map<std::string, wgpu::BindGroupLayout> m_BindGroupLayouts;

    std::map<std::string, BindGroupData> m_BindGroups;

    //wgpu::BlendState m_BlendState;

    wgpu::RenderPipeline m_Pipeline;

    std::unique_ptr<iEngine::ShaderModule> m_Shader;

    bool m_IsPrepared;
};