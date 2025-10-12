#include "iengine/core/Mesh.h"
#include "iengine/renderers/Context.h"

#include <iostream>
#include <cstring>

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
        
        std::cout << "Uploading mesh to GPU..." << std::endl;
        
        // 1. 获取顶点布局
        std::cout << "Getting vertex layout..." << std::endl;
        VertexLayout layout = getVertexLayout();
        std::cout << "Vertex layout created, stride: " << layout.arrayStride << std::endl;
        
        // 2. 构建交错缓冲区
        std::cout << "Building interleaved buffer..." << std::endl;
        std::vector<float> interleavedBuffer = buildInterleavedBuffer(layout);
        std::cout << "Interleaved buffer size: " << interleavedBuffer.size() << " floats" << std::endl;
        
        // 3. 清理旧缓冲区
        std::cout << "Cleaning old buffers..." << std::endl;
        if (vbo) {
            context->deleteBuffer(vbo);
            vbo = nullptr;
        }
        if (ibo && !geometry->indices.empty()) {
            context->deleteBuffer(ibo);
            ibo = nullptr;
        }
        
        // 4. 创建新缓冲区
        std::cout << "Creating new buffers..." << std::endl;
        if (!interleavedBuffer.empty()) {
            std::cout << "Creating vertex buffer..." << std::endl;
            vbo = context->createVertexBuffer(interleavedBuffer.size() * sizeof(float));
            std::cout << "Writing vertex buffer data..." << std::endl;
            context->writeBuffer(vbo, interleavedBuffer.data(), 
                               interleavedBuffer.size() * sizeof(float), 0);
            std::cout << "Vertex buffer created and written" << std::endl;
        }
        
        if (!geometry->indices.empty()) {
            std::cout << "Creating index buffer..." << std::endl;
            ibo = context->createIndexBuffer(geometry->indices.size() * sizeof(unsigned int));
            std::cout << "Writing index buffer data..." << std::endl;
            context->writeBuffer(ibo, geometry->indices.data(), 
                               geometry->indices.size() * sizeof(unsigned int), 0);
            std::cout << "Index buffer created and written" << std::endl;
        }
        
        uploaded = true;
        std::cout << "Mesh uploaded successfully. Vertices: " << geometry->vertexCount 
                  << ", Indices: " << geometry->indexCount << std::endl;
    }
    
    std::vector<float> Mesh::buildInterleavedBuffer(const VertexLayout& layout) const {
        if (geometry->vertexCount == 0) {
            return {};
        }
        
        size_t strideInFloats = layout.arrayStride / sizeof(float);
        std::vector<float> interleavedBuffer(geometry->vertexCount * strideInFloats, 0.0f);
        
        // 为每个顶点组装数据
        for (size_t vertexIndex = 0; vertexIndex < geometry->vertexCount; ++vertexIndex) {
            size_t bufferIndex = vertexIndex * strideInFloats;
            
            // 处理每个属性
            for (const auto& attr : layout.attributes) {
                size_t attrOffsetInFloats = attr.offset / sizeof(float);
                
                // 根据属性名称获取数据
                const std::vector<float>* sourceData = nullptr;
                size_t componentCount = 3; // 默认为float3
                
                if (attr.name == "aPosition") {
                    sourceData = &geometry->vertices;
                    componentCount = 3;
                } else if (attr.name == "aNormal") {
                    sourceData = &geometry->normals;
                    componentCount = 3;
                } else if (attr.name == "aTexCoord") {
                    sourceData = &geometry->texCoords;
                    componentCount = 2;
                } else if (attr.name == "aColor0") {
                    sourceData = &geometry->colors0;
                    componentCount = 4;
                } else if (attr.name == "aColor1") {
                    sourceData = &geometry->colors1;
                    componentCount = 4;
                } else if (attr.name == "aTangent") {
                    sourceData = &geometry->tangents;
                    componentCount = 3;
                } else if (attr.name == "aBitangent") {
                    sourceData = &geometry->bitangents;
                    componentCount = 3;
                }
                
                // 复制数据
                if (sourceData && !sourceData->empty()) {
                    size_t sourceIndex = vertexIndex * componentCount;
                    for (size_t i = 0; i < componentCount && i < 4; ++i) {
                        if (sourceIndex + i < sourceData->size()) {
                            interleavedBuffer[bufferIndex + attrOffsetInFloats + i] = (*sourceData)[sourceIndex + i];
                        }
                    }
                }
            }
        }
        
        return interleavedBuffer;
    }
}