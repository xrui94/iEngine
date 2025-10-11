#include "../../include/iengine/lights/PointLight.h"

namespace iengine {

    PointLight::PointLight(const Vector3& position, const Color& color, float intensity, float range)
        : Light(color, intensity), position(position), range(range) {
        // Point light specific initialization
    }

    const Vector3& PointLight::getPosition() const {
        return position;
    }

    float PointLight::getRange() const {
        return range;
    }

    void PointLight::setPosition(const Vector3& position) {
        this->position = position;
    }

    void PointLight::setRange(float range) {
        this->range = range;
    }

} // namespace iengine