#pragma once

namespace iengine {
    class Vector3 {
    public:
        float x, y, z;
        
        Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
        
        void set(float x, float y, float z);
        Vector3 copy() const;
        
        Vector3& add(const Vector3& v);
        Vector3& sub(const Vector3& v);
        Vector3& multiplyScalar(float scalar);
        Vector3& divideScalar(float scalar);
        
        float length() const;
        Vector3& normalize();
        
        float dot(const Vector3& v) const;
        Vector3 cross(const Vector3& v) const;
        
        float distanceTo(const Vector3& v) const;
    };
}