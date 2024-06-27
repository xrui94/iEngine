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
    std::shared_ptr<Material> material,
    std::vector<std::shared_ptr<Texture>>& textures,
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

MeshType Model::GetMeshType()
{
    return m_MeshType;
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

bool Model::HasIndices()
{
    // m_Indices.size() > 0 ? return true : return false;
    if (m_Indices.size() > 0) return true;
    else return false;
}

bool Model::HasInstances()
{
    // m_Instances.size() > 0 ? return true : return false;
    if (m_Instances.size() > 0) return true;
    else return false;
}

uint32_t Model::GetIndicesCount()
{
    if (m_Indices.size() % 2 != 0)
    {
        std::cerr << "Invalid indice data. It must be a multiple of 2." << std::endl;
    }
    return static_cast<uint32_t>(m_Indices.size() / sizeof(uint16_t));
}
    
uint32_t Model::GetInstancesCount()
{
    // 确保数据量足够且是偶数  
    if (m_Instances.size() % 2 != 0)
    {
        std::cerr << "Invalid instance data. It must be a multiple of 2." << std::endl;
    }

    if (m_Instances.size() % 2 % 16 != 0)
    {
        std::cerr << "Invalid instance data. It must be a multiple of 16." << std::endl;
    }

    return static_cast<uint32_t>(m_Instances.size() / sizeof(float) / 16);
}

uint32_t Model::GetVerticesCount()
{
    return 0;
}

std::map<std::string, BufferLayout>& Model::GetVertexAttrsInfo()
{
    return m_VertexAttrsInfo;
}

BufferLayout& Model::GetInstanceInfo()
{
    return m_InstancesInfo;
}

uint32_t Model::GetVAO(std::string handle)
{
    return m_VAO[handle];
}

void Model::SetVAO(std::string handle, uint32_t* bufferObj)
{
    m_VAO[handle] = *bufferObj;
}

void Model::SetVBO(std::string handle, uint32_t* bufferObj)
{
    m_VBO[handle] = *bufferObj;
}

void Model::SetEBO(std::string handle, uint32_t* bufferObj)
{
    m_EBO[handle] = *bufferObj;
}

void Model::SetIBO(std::string handle, uint32_t* bufferObj)
{
    m_IBO[handle] = *bufferObj;
}