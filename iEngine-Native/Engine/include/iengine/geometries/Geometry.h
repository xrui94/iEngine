#pragma once

#include <vector>
#include <array>
#include <memory>

namespace iengine {
    class Geometry {
    public:
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texCoords;
        std::vector<float> colors0;
        std::vector<float> colors1;
        std::vector<float> tangents;
        std::vector<float> bitangents;
        std::vector<unsigned int> indices;
        
        struct BoundingBox {
            std::array<float, 3> min;
            std::array<float, 3> max;
        };
        
        BoundingBox boundingBox;
        size_t vertexCount = 0;
        size_t indexCount = 0;
        
        Geometry(const std::vector<float>& vertices,
                 const std::vector<unsigned int>& indices = {});
        
        Geometry(const std::vector<float>& vertices,
                 const std::vector<float>& normals,
                 const std::vector<float>& texCoords,
                 const std::vector<unsigned int>& indices = {});
        
    private:
        BoundingBox computeBoundingBox();
    };
}