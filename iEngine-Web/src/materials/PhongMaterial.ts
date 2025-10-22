import { Material } from './Material';
import { Color } from './Color';
import { Matrix4 } from '../math/Matrix4';
import type { Renderable } from '../renderers/Renderable';

import type { TextureInfo } from './Material';
import type { WebGPUShaderModule } from '../renderers/webgpu/WebGPUShaderModule';
import type { Mesh } from '../core/Mesh';
import type { RenderableComponent } from '../renderers/RenderableComponent';

export interface PhongMaterialParams {
    name?: string;
    shaderName?: string;
    color?: Color;
    specular?: Color;
    shininess?: number;

}

export class PhongMaterial extends Material {

    color: Color = Color.white;
    specular: Color = Color.white;
    shininess: number = 32;

    constructor(params: PhongMaterialParams = { shaderName: 'base_phong' }) {
        super(
            'phong', 
            params.shaderName ?? 'base_phong',
            params
        );
        if (params.color !== undefined) this.color = params.color;
        if (params.specular !== undefined) this.specular = params.specular;
        if (params.shininess !== undefined) this.shininess = params.shininess;
    }

    getRenderPipelineState() {
        return {};
    }

    getShaderMacroDefines(): Record<string, string | number | boolean> {
        const defines: Record<string, boolean> = {};
        return defines;
    }

    getUniforms(context: any, camera: any, component: Renderable, lights: any) {
        return {
            uModelViewMatrix: camera.getViewMatrix().multiply(component.worldTransform),
            uProjectionMatrix: camera.getProjectionMatrix(),
            // ...其它uniform
        };
    }

    getTextures(): TextureInfo {
        return {};
    }

    getWebGPURenderPipelineDescriptor(device: GPUDevice, shader: WebGPUShaderModule, mesh: Mesh): GPURenderPipelineDescriptor {
        return {
            layout: 'auto',
            vertex: {
                module: shader.getModule(),
                entryPoint: 'vs_main',
                buffers: [mesh.getVertexLayout()] as Iterable<GPUVertexBufferLayout>, // 你需要实现此方法
            },
            fragment: {
                module: shader.getModule(),
                entryPoint: 'fs_main',
                targets: [{
                    format: 'bgra8unorm', // 或根据 context.format
                }]
            },
            primitive: {
                topology: mesh.primitive.getGPUPrimitiveType(),
                cullMode: 'back',
                frontFace: 'ccw'
            }
        };
    }

}
