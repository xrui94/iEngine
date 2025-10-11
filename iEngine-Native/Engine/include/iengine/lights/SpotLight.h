#pragma once

#include "Light.h"
#include "../math/Vector3.h"

namespace iengine {
    class SpotLight : public Light {
    public:
        Vector3 position;   // 光源位置
        Vector3 direction;  // 光照方向
        float angle;        // 光照角度
        float range;        // 光照范围
        
        SpotLight(const Vector3& position = Vector3(0, 10, 0),
                 const Vector3& direction = Vector3(0, -1, 0),
                 float angle = 0.523599f, // π/6
                 const Color& color = Color(1.0f, 1.0f, 1.0f), 
                 float intensity = 1.0f,
                 float range = 100.0f);
        
        virtual ~SpotLight() = default;
        
        const Vector3& getPosition() const;
        const Vector3& getDirection() const;
        float getAngle() const;
        float getRange() const;
        
        void setPosition(const Vector3& position);
        void setDirection(const Vector3& direction);
        void setAngle(float angle);
        void setRange(float range);
    };
}