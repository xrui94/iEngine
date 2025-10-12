#include "iengine/lights/SpotLight.h"

#include <cmath>

namespace iengine {

    SpotLight::SpotLight(const Vector3& position, const Vector3& direction, float angle, const Color& color, float intensity, float range)
        : Light(color, intensity), position(position), direction(direction), angle(angle), range(range) {
        // Spot light specific initialization
    }

    const Vector3& SpotLight::getPosition() const {
        return position;
    }

    const Vector3& SpotLight::getDirection() const {
        return direction;
    }

    float SpotLight::getAngle() const {
        return angle;
    }

    float SpotLight::getRange() const {
        return range;
    }

    void SpotLight::setPosition(const Vector3& position) {
        this->position = position;
    }

    void SpotLight::setDirection(const Vector3& direction) {
        this->direction = direction;
    }

    void SpotLight::setAngle(float angle) {
        this->angle = angle;
    }

    void SpotLight::setRange(float range) {
        this->range = range;
    }

} // namespace iengine