#include "../../../include/iengine/shaders/wgsl/BaseMaterialShader.h"
#include <string>

namespace iengine {

    const std::string BaseMaterialShader::code = R"(
    // WebGPU 的 uniform buffer（UBO）必须是 16 字节对齐的，并且 struct 总大小也要对齐到 16 字节的倍数。
    struct Uniforms {
        modelViewMatrix : mat4x4<f32>,      // 64 bytes
        projectionMatrix : mat4x4<f32>,     // 64 bytes
        uBaseColor : vec3<f32>,             // 12 bytes
        _pad : f32,                         // 4 bytes (手动补齐)
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
        return vec4<f32>(uniforms.uBaseColor, 1.0);
    }
)";

} // namespace iengine