#include "../../../include/iengine/shaders/glsl/BaseWireframeShader.h"
#include <string>

namespace iengine {

    const std::string BaseWireframeShader::vertex = R"(
        attribute vec3 aPosition;
        uniform mat4 uModelViewMatrix;
        uniform mat4 uProjectionMatrix;
        
        void main() {
            gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);
        }
    )";

    const std::string BaseWireframeShader::fragment = R"(
        precision mediump float;
        uniform vec3 uColor;
        
        void main() {
            gl_FragColor = vec4(uColor, 0.5);
        }
    )";

} // namespace iengine