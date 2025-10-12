#include "iengine/core/Primitive.h"

namespace iengine {
    // 静态成员初始化
    std::map<std::string, VertexFormatInfo> Primitive::VertexFormatInfoMap = {
        {"float32", {1, 0x1406, false}},     // GL_FLOAT
        {"float32x2", {2, 0x1406, false}},   // GL_FLOAT
        {"float32x3", {3, 0x1406, false}},   // GL_FLOAT  
        {"float32x4", {4, 0x1406, false}},   // GL_FLOAT
        {"uint32", {1, 0x1405, false}},      // GL_UNSIGNED_INT
        {"uint16", {1, 0x1403, false}},      // GL_UNSIGNED_SHORT
        {"uint8", {1, 0x1401, false}},       // GL_UNSIGNED_BYTE
    };
    
    Primitive::Primitive(PrimitiveType type, std::shared_ptr<VertexLayout> layout)
        : type(type), vertexLayout(layout) {}
    
    unsigned int Primitive::getGLPrimitiveType() const {
        switch (type) {
            case PrimitiveType::POINTS: return 0x0000;         // GL_POINTS
            case PrimitiveType::LINES: return 0x0001;          // GL_LINES
            case PrimitiveType::LINE_LOOP: return 0x0002;      // GL_LINE_LOOP
            case PrimitiveType::LINE_STRIP: return 0x0003;     // GL_LINE_STRIP
            case PrimitiveType::TRIANGLES: return 0x0004;      // GL_TRIANGLES
            case PrimitiveType::TRIANGLE_STRIP: return 0x0005; // GL_TRIANGLE_STRIP
            case PrimitiveType::TRIANGLE_FAN: return 0x0006;   // GL_TRIANGLE_FAN
            default: return 0x0004; // 默认为GL_TRIANGLES
        }
    }
    
    std::string Primitive::getWebGPUPrimitiveType() const {
        switch (type) {
            case PrimitiveType::POINTS: return "point-list";
            case PrimitiveType::LINES: return "line-list";
            case PrimitiveType::LINE_STRIP: return "line-strip";
            case PrimitiveType::TRIANGLES: return "triangle-list";
            case PrimitiveType::TRIANGLE_STRIP: return "triangle-strip";
            default: return "triangle-list"; // 默认为triangle-list
        }
    }
    
    VertexFormatInfo Primitive::getVertexFormatInfo(const std::string& format) {
        auto it = VertexFormatInfoMap.find(format);
        if (it != VertexFormatInfoMap.end()) {
            return it->second;
        }
        
        // 默认返回float32x3
        return {3, 0x1406, false}; // GL_FLOAT
    }
}