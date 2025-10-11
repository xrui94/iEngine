#pragma once

namespace iengine {
    class Vector2 {
    public:
        float x, y;
        
        Vector2(float x = 0.0f, float y = 0.0f);
        
        void set(float x, float y);
        Vector2 copy() const;
        
        Vector2& add(const Vector2& v);
        Vector2& sub(const Vector2& v);
        Vector2& multiplyScalar(float scalar);
        Vector2& divideScalar(float scalar);
        
        float length() const;
        Vector2& normalize();
        
        float dot(const Vector2& v) const;
        
        float distanceTo(const Vector2& v) const;
    };
}