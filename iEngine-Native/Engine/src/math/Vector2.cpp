#include "../../include/iengine/math/Vector2.h"
#include <cmath>

namespace iengine {

    Vector2::Vector2(float x, float y) : x(x), y(y) {}

    void Vector2::set(float x, float y) {
        this->x = x;
        this->y = y;
    }

    Vector2 Vector2::copy() const {
        return Vector2(x, y);
    }

    Vector2& Vector2::add(const Vector2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2& Vector2::sub(const Vector2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2& Vector2::multiplyScalar(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& Vector2::divideScalar(float scalar) {
        if (scalar != 0.0f) {
            x /= scalar;
            y /= scalar;
        }
        return *this;
    }

    float Vector2::length() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2& Vector2::normalize() {
        float len = length();
        if (len > 0.0f) {
            x /= len;
            y /= len;
        }
        return *this;
    }

    float Vector2::dot(const Vector2& v) const {
        return x * v.x + y * v.y;
    }

    float Vector2::distanceTo(const Vector2& v) const {
        float dx = x - v.x;
        float dy = y - v.y;
        return std::sqrt(dx * dx + dy * dy);
    }

} // namespace iengine