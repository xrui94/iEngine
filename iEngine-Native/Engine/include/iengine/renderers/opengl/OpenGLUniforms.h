#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <functional>

namespace iengine {
    // 前向声明
    class OpenGLContext;
    class Texture;
    
    enum class UniformType {
        // 标量
        FLOAT,
        INT,
        UINT,
        BOOL,
        // 向量
        VEC2,
        VEC3,
        VEC4,
        IVEC2,
        IVEC3,
        IVEC4,
        UVEC2,
        UVEC3,
        UVEC4,
        BVEC2,
        BVEC3,
        BVEC4,
        // 矩阵
        MAT2,
        MAT3,
        MAT4,
        MAT2X3,
        MAT2X4,
        MAT3X2,
        MAT3X4,
        MAT4X2,
        MAT4X3,
        // 纹理
        SAMPLER2D,
        SAMPLERCUBE,
        SAMPLER2DARRAY,
        SAMPLER3D,
        SAMPLER2DSHADOW,
        SAMPLERCUBESHADOW,
        SAMPLER2DARRAYSHADOW,
        // OpenGL 支持的采样器对象类型（高级）
        ISAMPLER2D,
        USAMPLER2D,
        ISAMPLER3D,
        USAMPLER3D,
        ISAMPLERCUBE,
        USAMPLERCUBE
    };
    
    struct UniformInfo {
        std::string name;
        UniformType type;
        int arraySize = 0;
    };
    
    // 简化的 UniformValue 实现
    class UniformValue {
    public:
        enum class Type {
            FLOAT,
            INT,
            UINT,
            BOOL,
            VEC2,
            VEC3,
            VEC4,
            MAT4,
            TEXTURE
        };
        
        // 默认构造函数
        UniformValue() : type_(Type::FLOAT), float_(0.0f) {}
        
        UniformValue(float value) : type_(Type::FLOAT), float_(value) {}
        UniformValue(int value) : type_(Type::INT), int_(value) {}
        UniformValue(unsigned int value) : type_(Type::UINT), uint_(value) {}
        UniformValue(bool value) : type_(Type::BOOL), bool_(value) {}
        UniformValue(const std::vector<float>& value);
        UniformValue(const std::shared_ptr<Texture>& value) : type_(Type::TEXTURE), texture_(value) {}
        
        // 用于矩阵的构造函数
        static UniformValue fromMatrix4(const class Matrix4& matrix);
        static UniformValue fromVec3(float x, float y, float z);
        static UniformValue fromVec4(float x, float y, float z, float w);
        
        Type getType() const { return type_; }
        
        float asFloat() const { return float_; }
        int asInt() const { return int_; }
        unsigned int asUInt() const { return uint_; }
        bool asBool() const { return bool_; }
        const std::vector<float>& asVec() const { return vec_; }
        const std::shared_ptr<Texture>& asTexture() const { return texture_; }
        
    private:
        Type type_;
        union {
            float float_;
            int int_;
            unsigned int uint_;
            bool bool_;
        };
        std::vector<float> vec_;
        std::shared_ptr<Texture> texture_;
    };
    
    class OpenGLUniforms {
    public:
        OpenGLUniforms(std::shared_ptr<OpenGLContext> context, void* program);
        ~OpenGLUniforms();
        
        void set(const std::string& name, const UniformValue& value);
        void setUniforms(const std::map<std::string, UniformValue>& uniforms);
        void initUniformSetters(); // 移到public，供OpenGLShaderProgram调用
        
    private:
        std::shared_ptr<OpenGLContext> context_;
        void* program_;
        std::map<std::string, std::function<void(const UniformValue&)>> uniformSetters_;
        
        void setUniformByType(unsigned int type, int location, const UniformValue& value);
    };
}