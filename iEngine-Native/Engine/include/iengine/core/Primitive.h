#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

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
    
    // 顶点格式信息
    struct VertexFormatInfo {
        int size;      // 组件数量
        unsigned int type;  // OpenGL类型
        bool normalized;
    };
    
    class Primitive {
    public:
        PrimitiveType type;
        std::shared_ptr<VertexLayout> vertexLayout;
        
        Primitive(PrimitiveType type, std::shared_ptr<VertexLayout> layout = nullptr);
        
        // 获取OpenGL原始类型
        unsigned int getGLPrimitiveType() const;
        
        // 获取WebGPU原始类型（留作未来扩展）
        std::string getWebGPUPrimitiveType() const;
        
        // 获取顶点格式信息
        static VertexFormatInfo getVertexFormatInfo(const std::string& format);
        
        // 常用顶点格式信息映射
        static std::map<std::string, VertexFormatInfo> VertexFormatInfoMap;
    };
}