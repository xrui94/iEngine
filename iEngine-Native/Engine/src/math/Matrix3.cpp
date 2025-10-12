#include "iengine/math/Matrix3.h"
#include "iengine/math/Vector3.h"

#include <cmath>
#include <cstring>

namespace iengine {

    Matrix3::Matrix3() {
        setIdentity();
    }

    Matrix3::Matrix3(float m11, float m12, float m13, 
                     float m21, float m22, float m23, 
                     float m31, float m32, float m33) {
        elements[0] = m11; elements[1] = m12; elements[2] = m13;
        elements[3] = m21; elements[4] = m22; elements[5] = m23;
        elements[6] = m31; elements[7] = m32; elements[8] = m33;
    }

    Matrix3::Matrix3(const Matrix3& other) {
        std::memcpy(elements.data(), other.elements.data(), sizeof(float) * 9);
    }

    Matrix3& Matrix3::operator=(const Matrix3& other) {
        if (this != &other) {
            std::memcpy(elements.data(), other.elements.data(), sizeof(float) * 9);
        }
        return *this;
    }

    void Matrix3::set(float m11, float m12, float m13, 
                      float m21, float m22, float m23, 
                      float m31, float m32, float m33) {
        elements[0] = m11; elements[1] = m12; elements[2] = m13;
        elements[3] = m21; elements[4] = m22; elements[5] = m23;
        elements[6] = m31; elements[7] = m32; elements[8] = m33;
    }

    void Matrix3::setIdentity() {
        elements[0] = 1.0f; elements[1] = 0.0f; elements[2] = 0.0f;
        elements[3] = 0.0f; elements[4] = 1.0f; elements[5] = 0.0f;
        elements[6] = 0.0f; elements[7] = 0.0f; elements[8] = 1.0f;
    }

    Matrix3 Matrix3::operator*(const Matrix3& other) const {
        Matrix3 result;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result.elements[i * 3 + j] = 0.0f;
                for (int k = 0; k < 3; ++k) {
                    result.elements[i * 3 + j] += elements[i * 3 + k] * other.elements[k * 3 + j];
                }
            }
        }
        return result;
    }

    Matrix3& Matrix3::operator*=(const Matrix3& other) {
        *this = *this * other;
        return *this;
    }

    Vector3 Matrix3::operator*(const Vector3& vector) const {
        return Vector3(
            elements[0] * vector.x + elements[1] * vector.y + elements[2] * vector.z,
            elements[3] * vector.x + elements[4] * vector.y + elements[5] * vector.z,
            elements[6] * vector.x + elements[7] * vector.y + elements[8] * vector.z
        );
    }

    Matrix3 Matrix3::transpose() const {
        Matrix3 result;
        result.elements[0] = elements[0]; result.elements[1] = elements[3]; result.elements[2] = elements[6];
        result.elements[3] = elements[1]; result.elements[4] = elements[4]; result.elements[5] = elements[7];
        result.elements[6] = elements[2]; result.elements[7] = elements[5]; result.elements[8] = elements[8];
        return result;
    }

    float Matrix3::determinant() const {
        return elements[0] * (elements[4] * elements[8] - elements[5] * elements[7]) -
               elements[1] * (elements[3] * elements[8] - elements[5] * elements[6]) +
               elements[2] * (elements[3] * elements[7] - elements[4] * elements[6]);
    }

    Matrix3 Matrix3::inverse() const {
        float det = determinant();
        if (std::abs(det) < 1e-8f) {
            // 行列式为0，无法求逆
            return Matrix3();
        }

        float invDet = 1.0f / det;
        Matrix3 result;
        result.elements[0] = (elements[4] * elements[8] - elements[5] * elements[7]) * invDet;
        result.elements[1] = (elements[2] * elements[7] - elements[1] * elements[8]) * invDet;
        result.elements[2] = (elements[1] * elements[5] - elements[2] * elements[4]) * invDet;
        result.elements[3] = (elements[5] * elements[6] - elements[3] * elements[8]) * invDet;
        result.elements[4] = (elements[0] * elements[8] - elements[2] * elements[6]) * invDet;
        result.elements[5] = (elements[2] * elements[3] - elements[0] * elements[5]) * invDet;
        result.elements[6] = (elements[3] * elements[7] - elements[4] * elements[6]) * invDet;
        result.elements[7] = (elements[1] * elements[6] - elements[0] * elements[7]) * invDet;
        result.elements[8] = (elements[0] * elements[4] - elements[1] * elements[3]) * invDet;
        return result;
    }

    void Matrix3::setRotationX(float angle) {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        set(1.0f, 0.0f, 0.0f,
            0.0f, cosA, -sinA,
            0.0f, sinA, cosA);
    }

    void Matrix3::setRotationY(float angle) {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        set(cosA, 0.0f, sinA,
            0.0f, 1.0f, 0.0f,
            -sinA, 0.0f, cosA);
    }

    void Matrix3::setRotationZ(float angle) {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        set(cosA, -sinA, 0.0f,
            sinA, cosA, 0.0f,
            0.0f, 0.0f, 1.0f);
    }

    void Matrix3::setScale(float x, float y, float z) {
        set(x, 0.0f, 0.0f,
            0.0f, y, 0.0f,
            0.0f, 0.0f, z);
    }

    float Matrix3::getElement(int row, int col) const {
        return elements[row * 3 + col];
    }

    void Matrix3::setElement(int row, int col, float value) {
        elements[row * 3 + col] = value;
    }
    
    // 从TypeScript版本添加的方法
    Matrix3 Matrix3::identity() {
        return Matrix3(1.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f,
                       0.0f, 0.0f, 1.0f);
    }
    
    Matrix3 Matrix3::clone() const {
        return Matrix3(*this);
    }
    
    Matrix3& Matrix3::copy(const Matrix3& m) {
        *this = m;
        return *this;
    }
    
    Matrix3 Matrix3::multiply(const Matrix3& matrix) const {
        return *this * matrix;
    }

} // namespace iengine