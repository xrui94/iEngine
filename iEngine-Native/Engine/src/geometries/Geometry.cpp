#include "../../include/iengine/geometries/Geometry.h"
#include <algorithm>
#include <limits>

namespace iengine {
    Geometry::Geometry(const std::vector<float>& vertices,
                       const std::vector<unsigned int>& indices)
        : vertices(vertices), indices(indices) {
        vertexCount = vertices.size() / 3;
        indexCount = indices.size();
        boundingBox = computeBoundingBox();
    }
    
    Geometry::Geometry(const std::vector<float>& vertices,
                       const std::vector<float>& normals,
                       const std::vector<float>& texCoords,
                       const std::vector<unsigned int>& indices)
        : vertices(vertices), normals(normals), texCoords(texCoords), indices(indices) {
        vertexCount = vertices.size() / 3;
        indexCount = indices.size();
        boundingBox = computeBoundingBox();
    }
    
    Geometry::BoundingBox Geometry::computeBoundingBox() {
        BoundingBox box;
        box.min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
        box.max = {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
        
        for (size_t i = 0; i < vertices.size(); i += 3) {
            float x = vertices[i];
            float y = vertices[i + 1];
            float z = vertices[i + 2];
            
            box.min[0] = std::min(box.min[0], x);
            box.min[1] = std::min(box.min[1], y);
            box.min[2] = std::min(box.min[2], z);
            
            box.max[0] = std::max(box.max[0], x);
            box.max[1] = std::max(box.max[1], y);
            box.max[2] = std::max(box.max[2], z);
        }
        
        return box;
    }
}