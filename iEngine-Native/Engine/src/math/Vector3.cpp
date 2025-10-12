#include "iengine/math/Vector3.h"

#include <cmath>

namespace iengine {
    Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    void Vector3::set(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    Vector3 Vector3::copy() const {
        return Vector3(x, y, z);
    }
    
    Vector3& Vector3::add(const Vector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    
    Vector3& Vector3::sub(const Vector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    
    Vector3& Vector3::multiplyScalar(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    Vector3& Vector3::divideScalar(float scalar) {
        return multiplyScalar(1.0f / scalar);
    }
    
    float Vector3::length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    Vector3& Vector3::normalize() {
        return divideScalar(length() > 0 ? length() : 1.0f);
    }
    
    float Vector3::dot(const Vector3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }
    
    Vector3 Vector3::cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
    
    float Vector3::distanceTo(const Vector3& v) const {
        float dx = x - v.x;
        float dy = y - v.y;
        float dz = z - v.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }
}