#pragma once


#include "Enum.h"
#include "materials/Material.h"
#include "textures/Texture.h"

// 三维模型的数据结构抽象，同时也是渲染引擎标准的数据结构
// 所有数据都应按该Model类的数据结构来解析读取
// OpenGL绑定数据时，需要指定数据类型，因此这里为了解析方便，model中的数据均定义为uint8_t类型
class Model
{
public:
    Model();

    Model(
        MeshType meshType,
        std::vector<float> vertexAttrs,
        std::vector<uint16_t> indices,
        std::vector<float> instances,
        std::shared_ptr<Material> pMaterial,
        std::vector<std::shared_ptr<Texture>>& textures,
        std::map<std::string, BufferLayout> VertexAttrsInfo,
        BufferLayout InstancesInfo);

    ~Model() = default;

public:

    MeshType GetMeshType();
    void SetMeshType(MeshType meshType) { m_MeshType = meshType; } 

    std::vector<uint8_t>& GetBBox() { return m_BBox; }
    // std::vector<float>& GetBBox() { return m_BBox; }

    void SetVertexAttrsInfo(std::map<std::string, BufferLayout>& vertexAttrsInfo) { m_VertexAttrsInfo = vertexAttrsInfo; }
    std::map<std::string, BufferLayout>& GetVertexAttrsInfo();

    std::vector<uint8_t>& GetVertexAttributes() { 
        // if (m_VertexAttrs.size() > 0){
        //     std::vector<float> vertices;
        //     TypeConverter::uInt8Array2Float32Array(m_VertexAttrs, vertices);
        //     for(auto& v : vertices)
        //     {
        //         std::cout << "vertex:\t" << v << std::endl;
        //     }
        // }
        return m_VertexAttrs; 
    }
    // std::vector<float>& GetVertexAttributes() { return m_VertexAttrs; }

    std::vector<uint8_t>& GetIndices() {
        // if (m_Indices.size() > 0){
        //     std::vector<uint16_t> indices;
        //     TypeConverter::uInt8Array2Uint16Array(m_Indices, indices);
        //     for(auto& i : indices)
        //     {
        //         std::cout << "index:\t" << i << std::endl;
        //     }
        // }

        return m_Indices;
    }
    // std::vector<uint16_t>& GetIndices() { return m_Indices; }
    void SetIndices(std::vector<uint8_t>& indices) { m_Indices = indices; }


    void SetInstancesInfo(BufferLayout& instancesInfo) { m_InstancesInfo = instancesInfo; }
    BufferLayout& GetInstanceInfo();

    std::vector<uint8_t>& GetInstances() { return m_Instances; }
    // std::vector<float>& GetInstances() { return m_Instances; }
    void SetInstances(std::vector<uint8_t>& instances) { m_Instances = instances; }

    void SetMaterials(std::shared_ptr<Material> pMaterial);
    std::shared_ptr<Material> GetMaterials();

    std::vector<std::shared_ptr<Texture>>& GetTextures() { return m_Textures; }
    void SetTextures(std::vector<std::shared_ptr<Texture>>& textures) { m_Textures = textures; }

    // std::shared_ptr<Bone> GetBones(std::vector<std::shared_ptr<Bone>> bone);

public:
    bool HasIndices();

    bool HasInstances();

    void SetShader(std::string handle, std::shared_ptr<ShaderProgram>);
    std::shared_ptr<ShaderProgram> GetShader(std::string handle);

    uint32_t GetIndicesCount();
    
    uint32_t GetInstancesCount();

    uint32_t GetVerticesCount();

    uint32_t GetVAO(std::string handle);
    
    void SetVAO(std::string handle, uint32_t* bufferObj);

    void SetVBO(std::string handle, uint32_t* bufferObj);

    void SetEBO(std::string handle, uint32_t* bufferObj);

    void SetIBO(std::string handle, uint32_t* bufferObj);

private:
    // name
    std::string m_Name;

    // boungding box
    std::vector<uint8_t> m_BBox;

    //
    MeshType m_MeshType;

    //
    std::vector<uint8_t> m_VertexAttrs;

    std::vector<uint8_t> m_Indices;

    std::vector<uint8_t> m_Instances;

    // material
    std::shared_ptr<Material> m_Material;

    // texture
    std::vector<std::shared_ptr<Texture>> m_Textures;

    // bone
    // std::vector<std::shared_ptr<Bone>> m_BoneBuffer;


    //
    std::map<std::string, std::shared_ptr<ShaderProgram>> m_Shaders;

    // 渲染属性Buffer
    std::map<std::string, uint32_t> m_VAO;
    std::map<std::string, uint32_t> m_VBO;
    std::map<std::string, uint32_t> m_EBO;
    std::map<std::string, uint32_t> m_IBO;

    std::map<std::string, BufferLayout> m_VertexAttrsInfo;
    BufferLayout m_InstancesInfo;
};