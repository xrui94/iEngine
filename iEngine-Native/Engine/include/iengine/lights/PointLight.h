#pragma once

#include "Light.h"
#include "../math/Vector3.h"

namespace iengine {
    class PointLight : public Light {
    public:
        Vector3 position;   // 光源位置
        float range;        // 光照范围
        
        PointLight(const Vector3& position = Vector3(0, 10, 0),
                  const Color& color = Color(1.0f, 1.0f, 1.0f), 
                  float intensity = 1.0f,
                  float range = 100.0f);
        
        virtual ~PointLight() = default;
        
        const Vector3& getPosition() const;
        float getRange() const;
        
        void setPosition(const Vector3& position);
        void setRange(float range);
    };
}