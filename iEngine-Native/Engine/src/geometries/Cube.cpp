#include "iengine/geometries/Cube.h"

namespace iengine {
    Cube::Cube(float size) : Geometry(
        // 顶点数据 (x, y, z) - 24个顶点，每个面4个顶点
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
        // 法线数据 (nx, ny, nz) - 与Web版本一致
        {
            // 前面 (每个顶点的法线)
            0, 0, 1,  0, 0, 1,  0, 0, 1,  0, 0, 1,
            // 后面
            0, 0, -1,  0, 0, -1,  0, 0, -1,  0, 0, -1,
            // 顶面
            0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,
            // 底面
            0, -1, 0,  0, -1, 0,  0, -1, 0,  0, -1, 0,
            // 右面
            1, 0, 0,  1, 0, 0,  1, 0, 0,  1, 0, 0,
            // 左面
            -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0
        },
        // 纹理坐标数据 (u, v) - 与Web版本一致
        {
            // 前面
            0, 0,  1, 0,  1, 1,  0, 1,
            // 后面
            1, 0,  1, 1,  0, 1,  0, 0,
            // 顶面
            0, 1,  0, 0,  1, 0,  1, 1,
            // 底面
            1, 1,  0, 1,  0, 0,  1, 0,
            // 右面
            0, 0,  0, 1,  1, 1,  1, 0,
            // 左面
            1, 0,  0, 0,  0, 1,  1, 1
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