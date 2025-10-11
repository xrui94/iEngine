export const BaseWireframeShader = {
    vertex: `
        attribute vec3 aPosition;
        uniform mat4 uModelViewMatrix;
        uniform mat4 uProjectionMatrix;
        
        void main() {
            gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);
        }
    `,
    fragment: `
        precision mediump float;
        uniform vec3 uColor;
        
        void main() {
            gl_FragColor = vec4(uColor, 0.5);
        }
    `
};