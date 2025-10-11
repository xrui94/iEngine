#include "../../../include/iengine/shaders/wgsl/BaseWireframeShader.h"
#include <string>

namespace iengine {

    const std::string BaseWireframeShader::code = R"(
struct Uniforms {
    modelViewMatrix : mat4x4<f32>,
    projectionMatrix : mat4x4<f32>,
    color : vec3<f32>,
    _pad : f32,
};

@group(0) @binding(0) var<uniform> uniforms : Uniforms;

struct VertexInput {
    @location(0) aPosition : vec3<f32>,
};

struct VertexOutput {
    @builtin(position) Position : vec4<f32>,
};

@vertex
fn vs_main(input : VertexInput) -> VertexOutput {
    var output : VertexOutput;
    output.Position = uniforms.projectionMatrix * uniforms.modelViewMatrix * vec4<f32>(input.aPosition, 1.0);
    return output;
}

@fragment
fn fs_main(input : VertexOutput) -> @location(0) vec4<f32> {
    // 半透明线框
    return vec4<f32>(uniforms.color, 0.5);
}
)";

} // namespace iengine