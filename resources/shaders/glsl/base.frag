#version 330 core

varying vec3 v_normal;
varying vec4 v_color;

uniform vec3 u_ambientLight;
uniform vec3 u_diffuseLight;
uniform vec3 u_lightDirection;

void main() {
    vec3 normal = normalize(v_normal);
    float fakeLight = dot(u_lightDirection, normal) * 0.5 + 0.5;
    vec4 diffuse = vec4((u_ambientLight + u_diffuseLight), 1.0) * v_color;
    gl_FragColor = vec4(diffuse.rgb * fakeLight, diffuse.a);

    // gl_FragColor = v_color;
}