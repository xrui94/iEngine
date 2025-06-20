import { ShaderLib } from "../shaders/ShaderLib";
import { WebGLConstants } from "../renderers/webgl/WebGLConstants";
import { WebGLShaderProgram } from "../renderers/webgl/WebGLShaderProgram";
import { WebGPUShaderModule } from "../renderers/webgpu/WebGPUShaderModule";

import type { GraphicsAPI } from "../renderers/Renderer";
import type { ShaderVariantOptions } from "../shaders/ShaderLib";
import type { Texture } from "../textures/Texture";
import type { WebGLContext } from "../renderers/webgl/WebGLContext";
import type { WebGPUContext } from "../renderers/webgpu/WebGPUContext";
import type { WebGLRenderPipelineState } from "../renderers/webgl/WebGLRenderPipeline";

export type TextureInfo = Record<string, Texture>;

export abstract class Material {
    public name: string;
    public shaderName: string;
    protected _shaderDefines: Record<string, string | number | boolean> = {};

    public shaderProgram: WebGLShaderProgram | null = null;
    public shaderModule: WebGPUShaderModule | null = null;

    depthTest: boolean = true;
    depthWrite: boolean = true;
    depthFunc: number = WebGLConstants.LEQUAL;
    transparent: boolean = true;
    doubleSided: boolean = true;

    constructor(name: string = 'default', shaderName: string = 'basic', params?: Partial<Material>) {
        this.name = name;
        this.shaderName = shaderName;
        if (params) Object.assign(this, params);
    }

    /**
     * 获取shader变体源码
     * @param backend 
     * @returns 
     */
    // getShaderVariant(backend: GraphicsAPI) {
    //     return ShaderLib.getVariant(this.shaderName, backend, this._shaderDefines);
    // }

    public getShader(
        device: WebGLContext | WebGPUContext,
        backend: GraphicsAPI,
        options?: ShaderVariantOptions
    ): WebGLShaderProgram | WebGPUShaderModule {
        const variant = ShaderLib.getVariant(this.shaderName, backend, options);
        if (!variant) {
            throw 'Failed to get shader from material.';
        }
        if (backend === 'webgl1' || backend === 'webgl2') {
            const glsl = variant.webgl;
            if (!glsl || !glsl.vertCode || !glsl.fragCode) {
                throw 'Failed to get webgpu shader from material.';
            }
            if (!this.shaderProgram) {
                this.shaderProgram = new WebGLShaderProgram(
                    device as WebGLContext,  // 类型断言
                    glsl.vertCode,
                    glsl.fragCode,
                    glsl.defines ?? {},
                    glsl.uniforms ?? [],
                );
            }
            return this.shaderProgram;
        } else {
            const wgsl = variant.webgpu;
            if (!wgsl || !wgsl.code) {
                throw 'Failed to get webgpu shader from material.';
            }
            if (!this.shaderModule) {
                this.shaderModule = new WebGPUShaderModule(
                    device as WebGPUContext,
                    wgsl.code,
                    wgsl.bindGroupEntryInfos ?? {}
                );
            }
            return this.shaderModule;
        }
    }

    // 声明抽象方法，要求子类必须实现
    abstract getShaderMacroDefines(): Record<string, string | number | boolean>;

    // 声明抽象方法，要求子类必须实现
    abstract getUniforms(context: any, camera: any, model: any, lights: any): Record<string, any>;

    abstract getTextures(): TextureInfo;

    abstract getRenderPipelineState(): WebGLRenderPipelineState;
}
