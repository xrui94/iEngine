#pragma once

#include "Light.h"
#include "../math/Vector3.h"

namespace iengine {
    class AmbientLight : public Light {
    public:
        AmbientLight(const Color& color = Color(1.0f, 1.0f, 1.0f), float intensity = 1.0f);
        virtual ~AmbientLight() = default;
    };
}