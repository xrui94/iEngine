#include "iengine/shaders/glsl/BaseMaterialShader.h"

#include <string>

namespace iengine {

    const std::string BaseMaterialShader::vertex = R"(#version 330 core
layout (location = 0) in vec3 aPosition;
uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;

void main() {
    gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);
}
)";

    const std::string BaseMaterialShader::fragment = R"(#version 330 core
out vec4 FragColor;
uniform vec3 uBaseColor;

void main() {
    FragColor = vec4(uBaseColor, 1.0);
}
)";

} // namespace iengine
