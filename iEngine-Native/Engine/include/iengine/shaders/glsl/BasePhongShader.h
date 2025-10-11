#ifndef IENGINE_BASE_PHONG_SHADER_H
#define IENGINE_BASE_PHONG_SHADER_H

#include <string>

namespace iengine {

    class BasePhongShader {
    public:
        static const std::string vertex;
        static const std::string fragment;
    };

} // namespace iengine

#endif // IENGINE_BASE_PHONG_SHADER_H