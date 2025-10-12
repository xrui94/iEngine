#pragma once

#include <memory>
#include <vector>
#include <array>
#include <string>
#include <map>
#include "../geometries/Geometry.h"
#include "../core/Primitive.h"

namespace iengine {
    // 前向声明
    class Context;
    
    class Mesh {
    public:
        std::shared_ptr<Geometry> geometry;
        std::shared_ptr<Primitive> primitive;
        bool uploaded = false;
        
        std::array<float, 16> transform = {{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        }};
        
        Mesh(std::shared_ptr<Geometry> geometry, std::shared_ptr<Primitive> primitive);
        
        bool hasNormal() const;
        bool hasUV() const;
        bool hasColor0() const;
        bool hasColor1() const;
        bool hasTangent() const;
        bool hasBitangent() const;
        
        VertexLayout getVertexLayout() const;
        
        std::map<std::string, bool> getShaderMacroDefines() const;
        
        void upload(std::shared_ptr<Context> context, bool force = false);
        
        // 构建交错缓冲区
        std::vector<float> buildInterleavedBuffer(const VertexLayout& layout) const;
        
    private:
        std::vector<std::pair<std::string, std::vector<float>>> vertexAttributeDataMap;
        
        // OpenGL/WebGPU资源
        void* vbo = nullptr;  // 顶点缓冲区对象
        void* ibo = nullptr;  // 索引缓冲区对象
    };
}