#include "../../../include/iengine/shaders/glsl/BaseMaterialShader.h"
#include <string>

namespace iengine {

    const std::string BaseMaterialShader::vertex = R"(
        attribute vec3 aPosition;
        uniform mat4 uModelViewMatrix;
        uniform mat4 uProjectionMatrix;
        
        void main() {
            gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);
        }
    )";

    const std::string BaseMaterialShader::fragment = R"(
        precision mediump float;
        uniform vec3 uBaseColor;
        
        void main() {
            gl_FragColor = vec4(uBaseColor, 1.0);
        }
    )";

} // namespace iengine