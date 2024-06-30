#pragma once


#include "Enum.h"
#include "VertexAttrs.h"
#include "Mesh.h"
#include "materials/Material.h"
#include "textures/Texture.h"
#include "renderers/opengl/ShaderProgram.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

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
        std::unique_ptr<Material> material,
        std::vector<std::shared_ptr<Texture>>& textures,
        std::unordered_map<std::string, BufferLayout> VertexAttrsInfo,
        BufferLayout InstancesInfo);

    Model(
        std::unique_ptr<Mesh> mesh,
        std::unique_ptr<Material> material,
        std::vector<std::shared_ptr<Texture>>& textures,
        std::unordered_map<std::string, BufferLayout> VertexAttrsInfo,
        BufferLayout InstancesInfo);

    ~Model() = default;

public:

    void SetMaterials(std::unique_ptr<Material> pMaterial);
    std::unique_ptr<Material> GetMaterials();

    std::vector<std::shared_ptr<Texture>>& GetTextures() { return m_Textures; }
    void SetTextures(std::vector<std::shared_ptr<Texture>>& textures) { m_Textures = textures; }

    // std::unique_ptr<Bone> GetBones(std::vector<std::unique_ptr<Bone>> bone);

    void SetShader(std::string handle, std::shared_ptr<ShaderProgram>);
    std::shared_ptr<ShaderProgram> GetShader(std::string handle);

private:
    //  id
    std::string m_ID;

    // name
    std::string m_Name;

    // mesh
    std::unique_ptr<Mesh> m_Mesh;

    // material
    std::unique_ptr<Material> m_Material;

    // texture
    std::vector<std::shared_ptr<Texture>> m_Textures;

    // bone
    // std::vector<std::unique_ptr<Bone>> m_BoneBuffer;

    // shader
    std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> m_Shaders;
};