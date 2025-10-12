#include "iengine/materials/Color.h"

#include <algorithm>
#include <cmath>

namespace iengine {

    Color::Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f), x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

    Color::Color(float r, float g, float b, float a) 
        : r(r), g(g), b(b), a(a) {
        x = r;
        y = g;
        z = b;
        w = a;
    }

    Color Color::clone() const {
        return Color(r, g, b, a);
    }

    Color Color::lerp(const Color& target, float t) const {
        return Color(
            r + (target.r - r) * t,
            g + (target.g - g) * t,
            b + (target.b - b) * t,
            a + (target.a - a) * t
        );
    }

    Color Color::operator*(float scalar) const {
        return Color(r * scalar, g * scalar, b * scalar, a * scalar);
    }

    Color Color::operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    Color& Color::operator+=(const Color& other) {
        r += other.r;
        g += other.g;
        b += other.b;
        a += other.a;
        x = r;
        y = g;
        z = b;
        w = a;
        return *this;
    }

    Color& Color::operator*=(float scalar) {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        a *= scalar;
        x = r;
        y = g;
        z = b;
        w = a;
        return *this;
    }

    float Color::getR() const { return r; }
    float Color::getG() const { return g; }
    float Color::getB() const { return b; }
    float Color::getA() const { return a; }

    void Color::setR(float r) { this->r = r; x = r; }
    void Color::setG(float g) { this->g = g; y = g; }
    void Color::setB(float b) { this->b = b; z = b; }
    void Color::setA(float a) { this->a = a; w = a; }

    void Color::set(float r, float g, float b, float a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
        this->x = r;
        this->y = g;
        this->z = b;
        this->w = a;
    }

    // 预定义颜色常量
    const Color Color::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
    const Color Color::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::RED(1.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::GREEN(0.0f, 1.0f, 0.0f, 1.0f);
    const Color Color::BLUE(0.0f, 0.0f, 1.0f, 1.0f);
    const Color Color::YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
    const Color Color::MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);
    const Color Color::CYAN(0.0f, 1.0f, 1.0f, 1.0f);

} // namespace iengine