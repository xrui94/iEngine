#include "Model.h"

#include <iostream>

Model::Model() : m_MeshType(MeshType::MT_UNKNOWN)
{
}

Model::Model(
    MeshType meshType,
    std::vector<float> vertexAttrs,
    std::vector<uint16_t> indices,
    std::vector<float> instances,
    std::unique_ptr<Material> material,
    std::vector<std::unique_ptr<Texture>>& textures,
    std::map<std::string, BufferLayout> vertexAttrsInfo,
    BufferLayout instancesInfo)
{
    m_MeshType = meshType;

    m_VertexAttrs.resize(sizeof(float) * vertexAttrs.size());
    memcpy(m_VertexAttrs.data(), vertexAttrs.data(), sizeof(float) * vertexAttrs.size());

    m_Indices.resize(sizeof(uint16_t) * indices.size());
    memcpy(m_Indices.data(), indices.data(), sizeof(uint16_t) * indices.size());

    m_Instances.resize(sizeof(float) * instances.size());
    memcpy(m_Instances.data(), instances.data(), sizeof(float) * instances.size());

    m_Material = material;
    m_Textures = textures;
    m_VertexAttrsInfo = vertexAttrsInfo;
    m_InstancesInfo = instancesInfo;
}

Model::Model(
    std::unique_ptr<Mesh> mesh,
    std::unique_ptr<Material> material,
    std::vector<std::unique_ptr<Texture>>& textures,
    std::unordered_map<std::string, BufferLayout> VertexAttrsInfo,
    BufferLayout InstancesInfo)
{
    m_Mesh = std::move(mesh);
    m_Material = std::move(material);
    m_Textures = textures;
    m_VertexAttrsInfo = vertexAttrsInfo;
    m_InstancesInfo = instancesInfo;
}

void Model::SetMaterials(std::shared_ptr<Material> pMaterial)
{
    m_Material = pMaterial;
}


std::shared_ptr<Material> Model::GetMaterials()
{
    return m_Material;
}

void Model::SetShader(std::string handle, std::shared_ptr<ShaderProgram> pShader)
{
    m_Shaders[handle] = pShader;
}

std::shared_ptr<ShaderProgram> Model::GetShader(std::string handle)
{
    return m_Shaders[handle];
}
