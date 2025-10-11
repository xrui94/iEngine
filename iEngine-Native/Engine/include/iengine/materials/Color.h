#pragma once

namespace iengine {
    class Color {
    public:
        float r, g, b, a;
        
        // 为了兼容 GLSL vec4
        float x, y, z, w;
        
        Color();
        Color(float r, float g, float b, float a = 1.0f);
        
        Color clone() const;
        Color lerp(const Color& target, float t) const;
        Color operator*(float scalar) const;
        Color operator+(const Color& other) const;
        Color& operator+=(const Color& other);
        Color& operator*=(float scalar);
        
        float getR() const;
        float getG() const;
        float getB() const;
        float getA() const;
        
        void setR(float r);
        void setG(float g);
        void setB(float b);
        void setA(float a);
        
        void set(float r, float g, float b, float a = 1.0f);
        
        // 预定义颜色常量
        static const Color WHITE;
        static const Color BLACK;
        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color YELLOW;
        static const Color MAGENTA;
        static const Color CYAN;
    };
}