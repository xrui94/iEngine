#include "../../include/iengine/lights/AmbientLight.h"

namespace iengine {

    AmbientLight::AmbientLight(const Color& color, float intensity)
        : Light(color, intensity) {
        // Ambient light specific initialization
    }

} // namespace iengine