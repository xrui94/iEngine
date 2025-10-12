#include "iengine/geometries/Triangle.h"

namespace iengine {
    Triangle::Triangle() : Geometry(
        // 顶点数据 (x, y, z)
        {
             0.0f,  0.5f, 0.0f,  // 顶点
            -0.5f, -0.5f, 0.0f,  // 左下
             0.5f, -0.5f, 0.0f   // 右下
        }
    ) {}
}