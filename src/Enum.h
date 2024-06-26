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
        BOOL,   // 0
        UINT,
        INT,
        FLOAT,

        //
        BVEC2,  // 16
        UVEC2,
        IVEC2,
        VEC2,

        //
        BVEC3,  // 32
        UVEC3,
        IVEC3,
        VEC3,

        //
        BVEC4,  // 48
        UVEC4,
        IVEC4,
        VEC4,

        //
        MAT2,   // 64
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