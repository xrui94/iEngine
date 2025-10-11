#pragma once

#include <array>
#include <memory>

namespace iengine {
    // 前向声明
    class Vector3;
    
    class Matrix3 {
    public:
        std::array<float, 9> elements;
        
        Matrix3();
        Matrix3(float m11, float m12, float m13, 
                float m21, float m22, float m23, 
                float m31, float m32, float m33);
        Matrix3(const Matrix3& other);
        
        Matrix3& operator=(const Matrix3& other);
        
        void set(float m11, float m12, float m13, 
                 float m21, float m22, float m23, 
                 float m31, float m32, float m33);
        void setIdentity();
        
        Matrix3 operator*(const Matrix3& other) const;
        Matrix3& operator*=(const Matrix3& other);
        Vector3 operator*(const Vector3& vector) const;
        
        Matrix3 transpose() const;
        float determinant() const;
        Matrix3 inverse() const;
        
        void setRotationX(float angle);
        void setRotationY(float angle);
        void setRotationZ(float angle);
        void setScale(float x, float y, float z);
        
        float getElement(int row, int col) const;
        void setElement(int row, int col, float value);
        
        static Matrix3 identity();
        Matrix3 clone() const;
        Matrix3& copy(const Matrix3& m);
        Matrix3 multiply(const Matrix3& matrix) const;
        
        int length() const { return static_cast<int>(elements.size()); }
    };
}