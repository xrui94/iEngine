#pragma once

#include <memory>
#include "../math/Vector3.h"
#include "../materials/Color.h"

namespace iengine {
    class Light {
    public:
        Color color;
        float intensity = 1.0f;
        
        Light(const Color& color = Color(1.0f, 1.0f, 1.0f), float intensity = 1.0f);
        virtual ~Light() = default;
    };
}