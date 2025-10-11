#include "../../include/iengine/lights/DirectionalLight.h"

namespace iengine {
    DirectionalLight::DirectionalLight(const Color& color, float intensity, const Vector3& direction)
        : Light(color, intensity), direction(direction) {}
}