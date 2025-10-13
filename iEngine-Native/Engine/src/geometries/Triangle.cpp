#include "iengine/geometries/Triangle.h"

namespace iengine {
    Triangle::Triangle() : Geometry(
        // 顶点数据 (x, y, z) - 与Web版本保持一致
        {
             0.0f,  1.0f, 0.0f,  // 顶点
            -1.0f, -1.0f, 0.0f,  // 左下
             1.0f, -1.0f, 0.0f   // 右下
        }
    ) {}
}