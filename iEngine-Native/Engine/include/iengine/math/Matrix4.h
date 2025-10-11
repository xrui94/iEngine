#pragma once

#include <array>
#include "Vector3.h"

namespace iengine {
    class Matrix3;
    
    class Matrix4 {
    public:
        std::array<float, 16> elements;
        
        Matrix4();
        Matrix4(const std::array<float, 16>& elements);
        
        static Matrix4 identity();
        static Matrix4 fromTranslation(const Vector3& offset);
        static Matrix4 fromScaling(const Vector3& scale);
        static Matrix4 fromRotation(const Vector3& axis, float angleRad);
        
        Matrix3 toMatrix3() const;
        
        Matrix4& setIdentity();
        Matrix4& multiply(const Matrix4& other);
        Matrix4& inverse();
        Matrix4& transpose();
        Matrix4& lookAt(float eyeX, float eyeY, float eyeZ,
                       float targetX, float targetY, float targetZ,
                       float upX, float upY, float upZ);
        Matrix4& perspective(float fov, float aspect, float near, float far);
        Matrix4& orthographic(float left, float right, float top, float bottom, float near, float far);
    };
}