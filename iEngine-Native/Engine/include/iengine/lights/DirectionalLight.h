#pragma once

#include "Light.h"
#include "../math/Vector3.h"

namespace iengine {
    class DirectionalLight : public Light {
    public:
        Vector3 direction;
        
        DirectionalLight(const Color& color = Color(1.0f, 1.0f, 1.0f), 
                        float intensity = 1.0f,
                        const Vector3& direction = Vector3(0.0f, -1.0f, 0.0f));
    };
}