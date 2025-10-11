#include "../../include/iengine/core/Mesh.h"

namespace iengine {
    Mesh::Mesh(std::shared_ptr<Geometry> geometry, std::shared_ptr<Primitive> primitive)
        : geometry(geometry), primitive(primitive) {
        // 初始化顶点属性数据映射
        vertexAttributeDataMap.push_back({"aPosition", geometry->vertices});
        if (!geometry->normals.empty()) {
            vertexAttributeDataMap.push_back({"aNormal", geometry->normals});
        }
        if (!geometry->texCoords.empty()) {
            vertexAttributeDataMap.push_back({"aTexCoord", geometry->texCoords});
        }
        if (!geometry->colors0.empty()) {
            vertexAttributeDataMap.push_back({"aColor0", geometry->colors0});
        }
        if (!geometry->colors1.empty()) {
            vertexAttributeDataMap.push_back({"aColor1", geometry->colors1});
        }
        if (!geometry->tangents.empty()) {
            vertexAttributeDataMap.push_back({"aTangent", geometry->tangents});
        }
        if (!geometry->bitangents.empty()) {
            vertexAttributeDataMap.push_back({"aBitangent", geometry->bitangents});
        }
    }
    
    bool Mesh::hasNormal() const {
        for (const auto& pair : vertexAttributeDataMap) {
            if (pair.first == "aNormal" && !pair.second.empty()) {
                return true;
            }
        }
        return false;
    }
    
    bool Mesh::hasUV() const {
        for (const auto& pair : vertexAttributeDataMap) {
            if (pair.first == "aTexCoord" && !pair.second.empty()) {
                return true;
            }
        }
        return false;
    }
    
    bool Mesh::hasColor0() const {
        for (const auto& pair : vertexAttributeDataMap) {
            if (pair.first == "aColor0" && !pair.second.empty()) {
                return true;
            }
        }
        return false;
    }
    
    bool Mesh::hasColor1() const {
        for (const auto& pair : vertexAttributeDataMap) {
            if (pair.first == "aColor1" && !pair.second.empty()) {
                return true;
            }
        }
        return false;
    }
    
    bool Mesh::hasTangent() const {
        for (const auto& pair : vertexAttributeDataMap) {
            if (pair.first == "aTangent" && !pair.second.empty()) {
                return true;
            }
        }
        return false;
    }
    
    bool Mesh::hasBitangent() const {
        for (const auto& pair : vertexAttributeDataMap) {
            if (pair.first == "aBitangent" && !pair.second.empty()) {
                return true;
            }
        }
        return false;
    }
    
    VertexLayout Mesh::getVertexLayout() const {
        // 优先用外部通过 Primitive 传进来的 VertexLayout 对象
        if (primitive->vertexLayout) {
            return *primitive->vertexLayout;
        }
        
        // 构建默认的顶点布局
        VertexLayout layout;
        layout.arrayStride = 0;
        
        // 计算 stride
        for (const auto& pair : vertexAttributeDataMap) {
            if (!pair.second.empty()) {
                // 简化处理，假设所有属性都是float3或float2
                size_t size = 3; // 默认是3个float (x,y,z)
                if (pair.first == "aTexCoord") {
                    size = 2; // 纹理坐标是2个float (u,v)
                } else if (pair.first == "aColor0" || pair.first == "aColor1") {
                    size = 4; // 颜色是4个float (r,g,b,a)
                }
                layout.arrayStride += size * sizeof(float);
            }
        }
        
        // 设置属性
        size_t offset = 0;
        int shaderLocation = 0;
        for (const auto& pair : vertexAttributeDataMap) {
            if (!pair.second.empty()) {
                VertexAttribute attr;
                attr.name = pair.first;
                attr.format = "float32x3"; // 默认格式
                attr.offset = offset;
                attr.shaderLocation = shaderLocation++;
                
                // 设置格式
                if (pair.first == "aTexCoord") {
                    attr.format = "float32x2";
                    offset += 2 * sizeof(float);
                } else if (pair.first == "aColor0" || pair.first == "aColor1") {
                    attr.format = "float32x4";
                    offset += 4 * sizeof(float);
                } else {
                    attr.format = "float32x3";
                    offset += 3 * sizeof(float);
                }
                
                layout.attributes.push_back(attr);
            }
        }
        
        return layout;
    }
    
    std::map<std::string, bool> Mesh::getShaderMacroDefines() const {
        std::map<std::string, bool> defines;
        if (hasNormal()) defines["HAS_NORMAL"] = true;
        if (hasUV()) defines["HAS_TEXCOORD"] = true;
        if (hasColor0()) defines["HAS_COLOR0"] = true;
        if (hasColor1()) defines["HAS_COLOR1"] = true;
        if (hasTangent()) defines["HAS_TANGENT"] = true;
        if (hasBitangent()) defines["HAS_BITANGENT"] = true;
        return defines;
    }
    
    void Mesh::upload(std::shared_ptr<Context> context, bool force) {
        if (uploaded && !force) return;
        
        // 这里应该是实际的上传逻辑
        // 由于这是框架代码，我们只设置标志
        uploaded = true;
    }
}