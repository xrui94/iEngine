import { Texture } from "../../textures/Texture";

import type { WebGLContext } from "./WebGLContext";

export type UniformType =
  // 标量
  | 'float'
  | 'int'
  | 'uint'
  | 'bool'
  // 向量
  | 'vec2'
  | 'vec3'
  | 'vec4'
  | 'ivec2'
  | 'ivec3'
  | 'ivec4'
  | 'uvec2'
  | 'uvec3'
  | 'uvec4'
  | 'bvec2'
  | 'bvec3'
  | 'bvec4'
  // 矩阵
  | 'mat2'
  | 'mat3'
  | 'mat4'
  | 'mat2x3'
  | 'mat2x4'
  | 'mat3x2'
  | 'mat3x4'
  | 'mat4x2'
  | 'mat4x3'
  // 纹理
  | 'sampler2D'
  | 'samplerCube'
  | 'sampler2DArray'
  | 'sampler3D'
  | 'sampler2DShadow'
  | 'samplerCubeShadow'
  | 'sampler2DArrayShadow'
  // WebGL2 支持的采样器对象类型（高级）
  | 'isampler2D'
  | 'usampler2D'
  | 'isampler3D'
  | 'usampler3D'
  | 'isamplerCube'
  | 'usamplerCube';

export type UniformInfo = {
    name: string;
    type: UniformType;
    arraySize?: number;
};

// type UniformValue =
//     | number
//     | number[]
//     | Float32Array
//     | { x: number; y: number }
//     | { x: number; y: number; z: number }
//     | { x: number; y: number; z: number; w: number }
//     | { elements: Float32Array }
//     | WebGLTexture
//     | UniformValue[]; // 支持数组

export type UniformValue =
  | number
  | number[]
  | Float32Array
  | Int32Array
  | Uint32Array
  | { x: number; y: number }
  | { x: number; y: number; z: number }
  | { x: number; y: number; z: number; w: number }
  | { elements: Float32Array } // 用于 Matrix4 等类封装
  | WebGLTexture
  | WebGLSampler
  | UniformValue[]; // 数组 uniform，例如 u_LightPositions[4]

export class WebGLUniforms {
    private context: WebGLContext;
    private program: WebGLProgram;
    private uniformSetters: Record<string, (value: UniformValue) => void> = {};

    constructor(context: WebGLContext, program: WebGLProgram) {
        this.context = context;
        this.program = program;
        this.initUniformSetters();
    }

    private initUniformSetters() {
        const program = this.program;
        const uniformCount = this.context.getUniformCount(program);

        for (let i = 0; i < uniformCount; ++i) {
            const info = this.context.getActiveUniform(program, i);
            if (!info) continue;
            const name = info.name.replace(/\[0\]$/, ""); // 兼容数组uniform名
            const loc = this.context.getUniformLocation(program, name);
            if (!loc) continue;

            const setter = (v: any) => this.context.setUniform(info.type, loc, v);
            this.uniformSetters[name] = setter;
        }
    }

    set(name: string, value: UniformValue): void {
        const setter = this.uniformSetters[name];
        if (setter) setter(value);
    }

    setUniforms(uniforms: Record<string, UniformValue>): void {
        let textureUnit = 0;
        const maxTextureUnits = this.context.maxTextureUnits;
        for (const key in uniforms) {
            // this.set(key, uniforms[key]);

            let value = uniforms[key];
            // 检查是否为 Texture 实例
            if (value instanceof Texture) {
                if (textureUnit >= maxTextureUnits) {
                    console.error(`Texture unit ${textureUnit} exceeds MAX_COMBINED_TEXTURE_IMAGE_UNITS (${maxTextureUnits})`);
                    continue;
                }

                value.unit = textureUnit;
                value.upload(this.context);

                //
                // if (value.needsUpdate()) {
                //     // value.upload(this.context);
                //     this.context.writeTexture(value.gpuTexture as WebGLTexture, value);
                //     value.markUpdated();
                //     value.unit = textureUnit;
                //     // this.context.bindTexture(value, textureUnit);
                // }

                if (value.needsUpdate()) {
                    // value.upload(this.context);
                    // value.unit = textureUnit;
                }
                // value.unit = textureUnit;

                //
                this.set(key, value);
                textureUnit++;
            } else {
                this.set(key, value);
            }
        }
    }

}