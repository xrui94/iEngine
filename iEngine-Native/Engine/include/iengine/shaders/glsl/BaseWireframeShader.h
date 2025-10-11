#ifndef IENGINE_BASE_WIREFRAME_SHADER_H
#define IENGINE_BASE_WIREFRAME_SHADER_H

#include <string>

namespace iengine {

    class BaseWireframeShader {
    public:
        static const std::string vertex;
        static const std::string fragment;
    };

} // namespace iengine

#endif // IENGINE_BASE_WIREFRAME_SHADER_H