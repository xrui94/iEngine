#include "../../../include/iengine/shaders/wgsl/BasePbrShader.h"
#include <string>

namespace iengine {

    const std::string BasePbrShader::code = R"(
    //
    struct Uniforms {
        modelViewMatrix : mat4x4<f32>,
        projectionMatrix : mat4x4<f32>,
        normalMatrix : mat4x4<f32>,

        // 相机位置
        cameraPos : vec4<f32>,

        // 基础颜色、金属光泽度、粗糙度
        baseColor : vec4<f32>,
        metallic : f32,
        roughness : f32,

        // 法线贴图
        normalScale : f32,

        // 环境遮蔽
        aoStrength : f32,

        // 自发光
        emissiveColor : vec4<f32>,
        emissiveIntensity : f32,
        // padding0 : vec3<f32>, // 或 vec3<f32>，但应 padding 成 vec4

        // 环境光
        ambientColor : vec4<f32>,

        // 平行光
        lightDir : vec4<f32>,
        lightColor : vec4<f32>,
        lightIntensity : f32,
        // padding1 : vec4<f32>, // 或 vec3<f32>，但应 padding 成 vec4
        // padding2: vec4<f32>, // <-- 新增，占 16 字节，解决 binding too small

        // ...如有其它uniform，继续补充
    };
    @group(0) @binding(0) var<uniform> uniforms : Uniforms;

    // 贴图采样器（可在 JS/TS 侧动态生成 binding 和 WGSL 代码（如没有贴图时不绑定））
    // 最常用的工程实践：始终绑定所有贴图和采样器，如果没有就绑定一张默认贴图（如 1x1 蓝色法线图）
    @group(0) @binding(1) var uBaseColorMap: texture_2d<f32>;
    @group(0) @binding(2) var uBaseColorSampler: sampler;

    @group(0) @binding(3) var uMetallicRoughnessMap: texture_2d<f32>;
    @group(0) @binding(4) var uMetallicRoughnessSampler: sampler;

    @group(0) @binding(5) var uNormalMap: texture_2d<f32>;
    @group(0) @binding(6) var uNormalSampler: sampler;

    @group(0) @binding(7) var uAoMap: texture_2d<f32>;
    @group(0) @binding(8) var uAoSampler: sampler;

    @group(0) @binding(9) var uEmissiveMap: texture_2d<f32>;
    @group(0) @binding(10) var uEmissiveSampler: sampler;

    struct VertexInput {
        @location(0) aPosition : vec3<f32>,
        @location(1) aNormal : vec3<f32>,
        @location(2) aTexCoord : vec2<f32>,
    };

    struct VertexOutput {
        @builtin(position) Position : vec4<f32>,
        @location(0) vWorldPos : vec3<f32>,
        @location(1) vNormal : vec3<f32>,
        @location(2) vTexCoord : vec2<f32>,
    };

    fn distributionGGX(N: vec3<f32>, H: vec3<f32>, roughness: f32) -> f32 {
        let a = roughness * roughness;
        let a2 = a * a;
        let NdotH = max(dot(N, H), 0.0);
        let NdotH2 = NdotH * NdotH;
        let denom = NdotH2 * (a2 - 1.0) + 1.0;
        return a2 / (3.1415926 * denom * denom);
    }

    fn geometrySchlickGGX(NdotV: f32, roughness: f32) -> f32 {
        let r = roughness + 1.0;
        let k = (r * r) / 8.0;
        return NdotV / (NdotV * (1.0 - k) + k);
    }

    fn geometrySmith(N: vec3<f32>, V: vec3<f32>, L: vec3<f32>, roughness: f32) -> f32 {
        let NdotV = max(dot(N, V), 0.0);
        let NdotL = max(dot(N, L), 0.0);
        let ggx1 = geometrySchlickGGX(NdotV, roughness);
        let ggx2 = geometrySchlickGGX(NdotL, roughness);
        return ggx1 * ggx2;
    }

    fn fresnelSchlick(cosTheta: f32, F0: vec3<f32>) -> vec3<f32> {
        return F0 + (vec3<f32>(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
    }

    fn mix_f32(x: f32, y: f32, a: f32) -> f32 {
        return x * (1.0 - a) + y * a;
    }

    fn mix_vec3(x: vec3<f32>, y: vec3<f32>, a: f32) -> vec3<f32> {
        return x * (1.0 - a) + y * a;
    }

    // jpg,png格式的图片(sRGB),需要gamma 解码
    fn srgbToLinear(c: vec3<f32>) -> vec3<f32> {
        return pow(c, vec3<f32>(2.4));
    }

    @vertex
    fn vs_main(input : VertexInput) -> VertexOutput {
        var output : VertexOutput;
        let worldPos = uniforms.modelViewMatrix * vec4<f32>(input.aPosition, 1.0);
        output.Position = uniforms.projectionMatrix * worldPos;
        output.vWorldPos = worldPos.xyz;
        // 仅取 normalMatrix 的前三行前三列
        let normalMat3 = mat3x3<f32>(
            uniforms.normalMatrix[0].xyz,
            uniforms.normalMatrix[1].xyz,
            uniforms.normalMatrix[2].xyz
        );
        output.vNormal = normalMat3 * input.aNormal;
        output.vTexCoord = input.aTexCoord;
        return output;
    }

    @fragment
    fn fs_main(input : VertexOutput) -> @location(0) vec4<f32> {
        // baseColorMap
        var baseColor = uniforms.baseColor.xyz;
        let sampledColor = textureSample(uBaseColorMap, uBaseColorSampler, input.vTexCoord).xyz;
        baseColor = baseColor * srgbToLinear(sampledColor);

        // metallic/roughness map
        let mrSample = textureSample(uMetallicRoughnessMap, uMetallicRoughnessSampler, input.vTexCoord);
        var metallic = uniforms.metallic * mrSample.b;
        var roughness = max(0.01, uniforms.roughness * mrSample.g); // 最小粗糙度

        var N = normalize(input.vNormal);

        // 法线贴图扰动
        // 避免出现：uNormalScale 不为 0 时，却依然使用默认的 1x1 法线贴图，这会导致
        // 用一张没有细节的法线贴图去扰动原本正确的法线，结果导致高光消失或偏移。
        if (uniforms.normalScale > 0.0) {
            var tangentNormal = textureSample(uNormalMap, uNormalSampler, input.vTexCoord).xyz * 2.0 - vec3<f32>(1.0, 1.0, 1.0);
            N = normalize(mix_vec3(N, tangentNormal, uniforms.normalScale));
        }

        let V = normalize(uniforms.cameraPos.xyz - input.vWorldPos);
        let L = normalize(-uniforms.lightDir.xyz);
        let H = normalize(V + L);

        // ao map
        var ao = 1.0;
        ao = mix_f32(1.0, textureSample(uAoMap, uAoSampler, input.vTexCoord).r, uniforms.aoStrength);

        // emissive map
        var emissive = uniforms.emissiveColor.xyz * uniforms.emissiveIntensity;
        emissive = emissive * textureSample(uEmissiveMap, uEmissiveSampler, input.vTexCoord).xyz;

        let NDF = distributionGGX(N, H, roughness);
        let G = geometrySmith(N, V, L, roughness);
        let F0 = mix_vec3(vec3<f32>(0.04), baseColor, metallic);
        var F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        F = clamp(F, vec3<f32>(0.0), vec3<f32>(0.98)); // 限制 Fresnel 放大

        let NdotL = max(dot(N, L), 0.0);

        let numerator = NDF * G * F;
        let denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.001;
        let specular = numerator / denominator;

        let kS = F;
        let kD = (vec3<f32>(1.0) - kS) * (1.0 - metallic);
        let diffuse = kD * baseColor / 3.1415926;

        let ambient = uniforms.ambientColor.xyz * baseColor * ao;

        let color = ambient + (diffuse + specular) * uniforms.lightColor.xyz * uniforms.lightIntensity * NdotL + emissive;

        // 直接输出颜色
        // return vec4<f32>(color, 1.0);

        // ===== 下边对颜色进行Tone Mapping 和 Gamma 校正 =====

        // 1. Tone Mapping（色调映射，Reinhard）
        // 物理渲染公式会产生大于1.0的高动态范围(HDR)颜色值，直接输出会导致高光区域“炸白”。
        // 如：uLightIntensity设置为10，导致高光区域过曝，呈现为白色。
        // Tone mapping 的作用是将 HDR 颜色压缩到 [0,1] 区间，模拟真实相机/人眼的响应，让高光不过曝、画面更自然。
        // 常见公式：
        // Reinhard: color_mapped = color / (color + 1.0)
        // 还有更高级的 ACES、Uncharted2等。
        let mapped = color / (color + vec3<f32>(1.0));

        // 2. Gamma 校正（2.2）
        // 屏幕显示是“伽马2.2”响应，线性空间的颜色直接显示会偏暗。
        // Gamma 校正一般是 color = pow(color, 1.0/2.2)，让颜色亮度更符合人眼感知。
        let gamma = 1.0 / 2.2;
        let mappedGamma = vec3<f32>(
            pow(mapped.x, gamma),
            pow(mapped.y, gamma),
            pow(mapped.z, gamma)
        );

        return vec4<f32>(mappedGamma, 1.0);

        // return vec4<f32>(normalize(N) * 0.5 + 0.5, 1.0);    // 测试法线是否正确
    }
)";

} // namespace iengine