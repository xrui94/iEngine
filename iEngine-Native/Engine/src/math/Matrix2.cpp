#include "iengine/math/Matrix2.h"
#include "iengine/math/Vector2.h"

#include <cmath>
#include <cstring>

namespace iengine {

    Matrix2::Matrix2() {
        setIdentity();
    }

    Matrix2::Matrix2(float m11, float m12, float m21, float m22) {
        elements[0] = m11; elements[1] = m12;
        elements[2] = m21; elements[3] = m22;
    }

    Matrix2::Matrix2(const Matrix2& other) {
        std::memcpy(elements.data(), other.elements.data(), sizeof(float) * 4);
    }

    Matrix2& Matrix2::operator=(const Matrix2& other) {
        if (this != &other) {
            std::memcpy(elements.data(), other.elements.data(), sizeof(float) * 4);
        }
        return *this;
    }

    void Matrix2::set(float m11, float m12, float m21, float m22) {
        elements[0] = m11; elements[1] = m12;
        elements[2] = m21; elements[3] = m22;
    }

    void Matrix2::setIdentity() {
        elements[0] = 1.0f; elements[1] = 0.0f;
        elements[2] = 0.0f; elements[3] = 1.0f;
    }

    Matrix2 Matrix2::operator*(const Matrix2& other) const {
        Matrix2 result;
        result.elements[0] = elements[0] * other.elements[0] + elements[1] * other.elements[2];
        result.elements[1] = elements[0] * other.elements[1] + elements[1] * other.elements[3];
        result.elements[2] = elements[2] * other.elements[0] + elements[3] * other.elements[2];
        result.elements[3] = elements[2] * other.elements[1] + elements[3] * other.elements[3];
        return result;
    }

    Matrix2& Matrix2::operator*=(const Matrix2& other) {
        *this = *this * other;
        return *this;
    }

    Vector2 Matrix2::operator*(const Vector2& vector) const {
        return Vector2(
            elements[0] * vector.x + elements[1] * vector.y,
            elements[2] * vector.x + elements[3] * vector.y
        );
    }

    Matrix2 Matrix2::transpose() const {
        Matrix2 result;
        result.elements[0] = elements[0]; result.elements[1] = elements[2];
        result.elements[2] = elements[1]; result.elements[3] = elements[3];
        return result;
    }

    float Matrix2::determinant() const {
        return elements[0] * elements[3] - elements[1] * elements[2];
    }

    Matrix2 Matrix2::inverse() const {
        float det = determinant();
        if (std::abs(det) < 1e-8f) {
            // 行列式为0，无法求逆
            return Matrix2();
        }

        float invDet = 1.0f / det;
        Matrix2 result;
        result.elements[0] = elements[3] * invDet;
        result.elements[1] = -elements[1] * invDet;
        result.elements[2] = -elements[2] * invDet;
        result.elements[3] = elements[0] * invDet;
        return result;
    }

    void Matrix2::setRotation(float angle) {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        set(cosA, -sinA, sinA, cosA);
    }

    void Matrix2::setScale(float x, float y) {
        set(x, 0.0f, 0.0f, y);
    }

    float Matrix2::getElement(int row, int col) const {
        return elements[row * 2 + col];
    }

    void Matrix2::setElement(int row, int col, float value) {
        elements[row * 2 + col] = value;
    }
    
    // 从TypeScript版本添加的方法
    Matrix2 Matrix2::identity() {
        return Matrix2(1.0f, 0.0f, 0.0f, 1.0f);
    }
    
    Matrix2 Matrix2::clone() const {
        return Matrix2(*this);
    }
    
    Matrix2& Matrix2::copy(const Matrix2& m) {
        *this = m;
        return *this;
    }
    
    Matrix2 Matrix2::multiply(const Matrix2& matrix) const {
        return *this * matrix;
    }

} // namespace iengine