#include "iengine/lights/Light.h"

namespace iengine {
    Light::Light(const Color& color, float intensity)
        : color(color), intensity(intensity) {}
}