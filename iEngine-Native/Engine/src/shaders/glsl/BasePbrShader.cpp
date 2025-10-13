#include "iengine/shaders/glsl/BasePbrShader.h"

#include <string>

namespace iengine {

    const std::string BasePbrShader::vertex = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;

#ifdef HAS_NORMAL
    layout(location = 1) in vec3 aNormal;
    out vec3 vNormal;
#endif

#ifdef HAS_TEXCOORD
    layout(location = 2) in vec2 aTexCoord;
    out vec2 vTexCoord;
#endif

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;

#ifdef HAS_NORMAL
    uniform mat3 uNormalMatrix;
#endif

out vec3 vWorldPos;

void main() {
    #ifdef HAS_NORMAL
        vNormal = normalize(uNormalMatrix * aNormal);
    #endif

    vec4 worldPos = uModelViewMatrix * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;

    #ifdef HAS_TEXCOORD
        vTexCoord = aTexCoord;
    #endif

    gl_Position = uProjectionMatrix * worldPos;
}
    )";

    const std::string BasePbrShader::fragment = R"(
        precision mediump float;

        #ifdef HAS_NORMAL
            varying vec3 vNormal;
        #endif

        varying vec3 vWorldPos;
        uniform vec3 uBaseColor;
        uniform float uMetallic;
        uniform float uRoughness;
        uniform vec3 uCameraPos;

        // 单方向光
        uniform vec3 uLightDir;
        uniform vec3 uLightColor;
        uniform float uLightIntensity;
        uniform vec3 uAmbientColor;

        #ifdef HAS_TEXCOORD
            varying vec2 vTexCoord;
        #endif

        uniform sampler2D uBaseColorMap;
        uniform sampler2D uMetallicRoughnessMap;
        uniform sampler2D uNormalMap;
        uniform float uNormalScale;
        uniform sampler2D uAoMap;
        uniform float uAoStrength;
        uniform sampler2D uEmissiveMap;

        // PBR核心函数
        float DistributionGGX(vec3 N, vec3 H, float roughness) {
            float a = roughness * roughness;
            float a2 = a * a;
            float NdotH = max(dot(N, H), 0.0);
            float NdotH2 = NdotH * NdotH;
            float denom = (NdotH2 * (a2 - 1.0) + 1.0);
            return a2 / (3.1415926 * denom * denom);
        }
        
        float GeometrySchlickGGX(float NdotV, float roughness) {
            float r = roughness + 1.0;
            float k = (r * r) / 8.0;
            return NdotV / (NdotV * (1.0 - k) + k);
        }
        
        float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
            float NdotV = max(dot(N, V), 0.0);
            float NdotL = max(dot(N, L), 0.0);
            float ggx1 = GeometrySchlickGGX(NdotV, roughness);
            float ggx2 = GeometrySchlickGGX(NdotL, roughness);
            return ggx1 * ggx2;
        }
        
        vec3 fresnelSchlick(float cosTheta, vec3 F0) {
            return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
        }

        void main() {
            // 采样 baseColor
            vec3 baseColor = uBaseColor;
            baseColor *= texture2D(uBaseColorMap, vTexCoord).rgb;

            // 先对贴图（含默认的1X1的uMetallicRoughnessMap贴图）进行采样 metallic/roughness
            // 再和uMetallic/uRoughness进行混合，避免用户设置了uMetallic/uRoughness，而未设置
            // uMetallicRoughnessMap时，用户的uMetallic/uRoughness的设置无法发挥作用。
            vec4 mrSample = texture2D(uMetallicRoughnessMap, vTexCoord);
            float metallic = uMetallic * mrSample.b;
            float roughness = max(0.01, uRoughness * mrSample.g); // 最小粗糙度

            #ifdef HAS_NORMAL
                vec3 N = normalize(vNormal);
            #else
                vec3 N = vec3(0.0, 0.0, 1.0);
            #endif

            // 法线贴图扰动
            // 避免出现：uNormalScale 不为 0 时，却依然使用默认的 1x1 法线贴图，这会导致
            // 用一张没有细节的法线贴图去扰动原本正确的法线，结果导致高光消失或偏移。
            if (uNormalScale > 0.0) {
                vec3 tangentNormal = texture2D(uNormalMap, vTexCoord).xyz * 2.0 - 1.0;
                N = normalize(mix(N, tangentNormal, uNormalScale));
            }

            vec3 V = normalize(uCameraPos - vWorldPos);
            vec3 L = normalize(-uLightDir);
            vec3 H = normalize(V + L);

            // 采样 ao
            float ao = 1.0;
            ao = mix(1.0, texture2D(uAoMap, vTexCoord).r, uAoStrength);

            // 采样 emissive
            vec3 emissive = vec3(0.0);
            emissive = texture2D(uEmissiveMap, vTexCoord).rgb;

            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F0 = mix(vec3(0.04), baseColor, metallic);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
            F = clamp(F, 0.0, 0.98); // 限制 Fresnel 放大

            float NdotL = max(dot(N, L), 0.0);

            vec3 numerator = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.001;
            vec3 specular = numerator / denominator;

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            vec3 diffuse = kD * baseColor / 3.1415926;

            // 环境光分量
            vec3 ambient = uAmbientColor * baseColor * ao;

            vec3 color = ambient + (diffuse + specular) * uLightColor * uLightIntensity * NdotL + emissive;

            // 直接输出颜色
            // gl_FragColor = vec4(color, 1.0);

            // 下边对颜色进行Tone Mapping 和 Gamma 校正，避免高光区域过曝
            
            // 1. Tone Mapping（色调映射，Reinhard）
            // 物理渲染公式会产生大于1.0的高动态范围(HDR)颜色值，直接输出会导致高光区域“炸白”。
            // 如：uLightIntensity设置为10，导致高光区域过曝，呈现为白色。
            // Tone mapping 的作用是将 HDR 颜色压缩到 [0,1] 区间，模拟真实相机/人眼的响应，让高光不过曝、画面更自然。
            // 常见公式：
            // Reinhard: color_mapped = color / (color + 1.0)
            // 还有更高级的 ACES、Uncharted2等。
            vec3 mapped = color / (color + vec3(1.0));

            // 2. Gamma 校正（2.2）
            // 屏幕显示是“伽马2.2”响应，线性空间的颜色直接显示会偏暗。
            // Gamma 校正一般是 color = pow(color, 1.0/2.2)，让颜色亮度更符合人眼感知。
            mapped = pow(mapped, vec3(1.0/2.2));

            gl_FragColor = vec4(mapped, 1.0);

            // 调试代码
            // gl_FragColor = vec4(texture2D(uBaseColorMap, vTexCoord).rgb, 1.0);   // 测试贴图是否正确
            // gl_FragColor = vec4(vTexCoord, 0.0, 1.0);   // 测试UV坐标是否正确
            // gl_FragColor = vec4(normalize(vNormal) * 0.5 + 0.5, 1.0);   // 测试法线是否正确
            // gl_FragColor = vec4(baseColor, 1.0);    // 测试基础颜色是否正确

        }
    )";

} // namespace iengine