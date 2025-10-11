#pragma once

#include "Geometry.h"
#include <memory>

namespace iengine {
    class Cube : public Geometry {
    public:
        Cube(float size = 1.0f);
    };
}