export const BasePhongShader = {
    vertex: `
        attribute vec3 aPosition;
        attribute vec3 aNormal;
        uniform mat4 uModelViewMatrix;
        uniform mat4 uProjectionMatrix;
        uniform mat3 uNormalMatrix;
        varying vec3 vNormal;
        varying vec3 vWorldPos;
        void main() {
            vNormal = normalize(uNormalMatrix * aNormal);
            vec4 worldPos = uModelViewMatrix * vec4(aPosition, 1.0);
            vWorldPos = worldPos.xyz;
            gl_Position = uProjectionMatrix * worldPos;
        }
    `,
    fragment: `
        precision mediump float;
        varying vec3 vNormal;
        varying vec3 vWorldPos;
        uniform vec3 uColor;
        uniform vec3 uSpecular;
        uniform float uShininess;
        uniform vec3 uCameraPos;

        // 单点光源
        uniform vec3 uLightPos;
        uniform vec3 uLightColor;
        uniform float uLightIntensity;

        void main() {
            vec3 N = normalize(vNormal);
            vec3 L = normalize(uLightPos - vWorldPos);
            vec3 V = normalize(uCameraPos - vWorldPos);
            vec3 H = normalize(L + V);

            float diff = max(dot(N, L), 0.0);
            float spec = pow(max(dot(N, H), 0.0), uShininess);

            vec3 diffuse = uColor * diff;
            vec3 specular = uSpecular * spec;

            vec3 color = (diffuse + specular) * uLightColor * uLightIntensity;

            gl_FragColor = vec4(color, 1.0);
        }
    `
};
