#pragma once

#include <array>
#include <memory>

namespace iengine {
    // 前向声明
    class Vector2;
    
    class Matrix2 {
    public:
        std::array<float, 4> elements;
        
        Matrix2();
        Matrix2(float m11, float m12, float m21, float m22);
        Matrix2(const Matrix2& other);
        
        Matrix2& operator=(const Matrix2& other);
        
        void set(float m11, float m12, float m21, float m22);
        void setIdentity();
        
        Matrix2 operator*(const Matrix2& other) const;
        Matrix2& operator*=(const Matrix2& other);
        Vector2 operator*(const Vector2& vector) const;
        
        Matrix2 transpose() const;
        float determinant() const;
        Matrix2 inverse() const;
        
        void setRotation(float angle);
        void setScale(float x, float y);
        
        float getElement(int row, int col) const;
        void setElement(int row, int col, float value);
        
        static Matrix2 identity();
        Matrix2 clone() const;
        Matrix2& copy(const Matrix2& m);
        Matrix2 multiply(const Matrix2& matrix) const;
    };
}