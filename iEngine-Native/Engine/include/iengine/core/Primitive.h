#pragma once

#include <memory>
#include <vector>
#include <string>

namespace iengine {
    enum class PrimitiveType {
        POINTS,
        LINES,
        LINE_LOOP,
        LINE_STRIP,
        TRIANGLES,
        TRIANGLE_STRIP,
        TRIANGLE_FAN
    };
    
    struct VertexAttribute {
        std::string name;
        std::string format;
        size_t offset;
        int shaderLocation;
    };
    
    struct VertexLayout {
        size_t arrayStride;
        std::vector<VertexAttribute> attributes;
    };
    
    class Primitive {
    public:
        PrimitiveType type;
        std::shared_ptr<VertexLayout> vertexLayout;
        
        Primitive(PrimitiveType type, std::shared_ptr<VertexLayout> layout = nullptr);
    };
}