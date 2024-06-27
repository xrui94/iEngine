#version 330 core 

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec4 a_color;

attribute mat4 a_instance;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform mat4 u_rotate;
uniform mat4 u_translate;
uniform mat4 u_scale;

varying vec3 v_normal;
varying vec4 v_color;

void main() {
    // float radianas = radians(-90.0);

    // float cos = cos(radian);
    // float sin = sin(radian);

    // mat4 mx = mat4(1,0,0,0, 0,cos,sin,0, 0,-sin,cos,0, 0,0,0,1);
    // mat4 my = mat4(cos,0,-sin,0, 0,1,0,0, sin,0,cos,0, 0,0,0,1);
    // mat4 mz = mat4(cos,sin,0,0, -sin,cos,0,0, 0,0,1,0, 0,0,0,1);
    // mat4 a_move = mat4(1.5,0,0,0, 0, 1.5, 0, 0, 0, 0, 1.5,0, 0, 0, 0,1);

    // gl_Position = u_projection * u_view * a_move * mx * u_model * u_translate * u_scale * u_rotate * vec4(a_position, 1.0);
    // gl_Position = u_projection * u_view * a_move * u_model * u_translate * u_scale * u_rotate * vec4(a_position, 1.0);
    // gl_Position = u_projection * u_view * mx * u_model * u_translate * u_scale * u_rotate * vec4(a_position, 1.0);
    
    // 判断是否进行实例化
    gl_Position = u_projection * u_view * u_model * u_translate * u_scale * u_rotate * a_instance * vec4(a_position, 1.0);

    v_normal = mat3(u_model) * a_normal;
    v_color = a_color;
}
