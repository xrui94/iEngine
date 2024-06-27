#pragma once

// 在 C++ 中，当你声明一个枚举类（也称为强类型枚举或作用域枚举）并试图为它指定一个底层类型时，
// 你需要确保所使用的类型是一个简单的、完整的类型，并且在该上下文中是可见的。uint8_t 是一个
// 定义在 <cstdint> 或 <stdint.h> 头文件中的类型，因此你需要包含这个头文件才能使用它。
#include <cstdint>

enum class PrimitiveType : uint8_t
{
    UNKNOWN = 0,
    POINTS,
    LINES,
    LINE_STRIP,
    LINE_LOOP,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
    TRIANGLES
};

enum class VertexDataType : uint8_t
{
    BYTE,
    UINT16,
    UINT32,
    UINT64,
    INT16,
    INT32,
    INT64,
    FLOAT32
};

// OpenGL Uniform数据类型枚举
enum class UniformDataType : uint8_t
{
    //
    BOOL, // 0
    UINT,
    INT,
    FLOAT,

    //
    BVEC2, // 16
    UVEC2,
    IVEC2,
    VEC2,

    //
    BVEC3, // 32
    UVEC3,
    IVEC3,
    VEC3,

    //
    BVEC4, // 48
    UVEC4,
    IVEC4,
    VEC4,

    //
    MAT2, // 64
    MAT2x3,
    MAT2x4,

    //
    MAT3x2, // 80
    MAT3,
    MAT3x4,

    //
    MAT4x2, // 96
    MAT4x3,
    MAT4,
};

enum class MeshType : uint8_t
{
    MT_UNKNOWN = 0,
    MT_FACE = 2,
    MT_LINE = 5,
    MT_POINT = 12,
    MT_TEXT = 4,
    // MT_REF = 0xFFFF,
    // MT_REFLIST = 0xFFFE,
};