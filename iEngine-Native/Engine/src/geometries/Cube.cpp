#include "iengine/geometries/Cube.h"

namespace iengine {
    Cube::Cube(float size) : Geometry(
        // 顶点数据 (x, y, z)
        {
            // 前面
            -size/2, -size/2,  size/2,
             size/2, -size/2,  size/2,
             size/2,  size/2,  size/2,
            -size/2,  size/2,  size/2,
            // 后面
            -size/2, -size/2, -size/2,
            -size/2,  size/2, -size/2,
             size/2,  size/2, -size/2,
             size/2, -size/2, -size/2,
            // 顶面
            -size/2,  size/2, -size/2,
            -size/2,  size/2,  size/2,
             size/2,  size/2,  size/2,
             size/2,  size/2, -size/2,
            // 底面
            -size/2, -size/2, -size/2,
             size/2, -size/2, -size/2,
             size/2, -size/2,  size/2,
            -size/2, -size/2,  size/2,
            // 右面
             size/2, -size/2, -size/2,
             size/2,  size/2, -size/2,
             size/2,  size/2,  size/2,
             size/2, -size/2,  size/2,
            // 左面
            -size/2, -size/2, -size/2,
            -size/2, -size/2,  size/2,
            -size/2,  size/2,  size/2,
            -size/2,  size/2, -size/2
        },
        // 索引数据
        {
             0,  1,  2,  0,  2,  3,  // 前面
             4,  5,  6,  4,  6,  7,  // 后面
             8,  9, 10,  8, 10, 11,  // 顶面
            12, 13, 14, 12, 14, 15,  // 底面
            16, 17, 18, 16, 18, 19,  // 右面
            20, 21, 22, 20, 22, 23   // 左面
        }
    ) {}
}