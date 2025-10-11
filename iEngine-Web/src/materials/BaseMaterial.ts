import { Material } from './Material';
import { Color } from './Color';
import { WebGLConstants } from '../renderers/webgl/WebGLConstants';

import type { TextureInfo } from './Material';
import type { WebGPUShaderModule } from '../renderers/webgpu/WebGPUShaderModule';
import type { Mesh } from '../core/Mesh';

export interface BaseMaterialParams {
    name?: string;
    shaderName: string;
    color?: Color;
}

export class BaseMaterial extends Material {
    color: Color = Color.white;

    // depthTest: boolean = true;
    // depthWrite: boolean = true;
    // depthFunc: number = WebGLConstants.LEQUAL;
    // transparent: boolean = true;
    // doubleSided: boolean = true;

    constructor(params: BaseMaterialParams = { shaderName: 'base' }) {
        super(
            'base',
            params.shaderName ?? 'base_pbr',
            params
        );
        if (params.color !== undefined) this.color = params.color;
    }

    /**
     * 设置材质颜色
     * @param r 红色分量 (0-1)
     * @param g 绿色分量 (0-1)
     * @param b 蓝色分量 (0-1)
     */
    setColor(r: number, g: number, b: number): void {
        this.color.set(r, g, b);
    }

    getRenderPipelineState() {
        return {};
    }

    getShaderMacroDefines(): Record<string, string | number | boolean> {
        const defines: Record<string, boolean> = {};
        return defines;
    }

    getUniforms(context: any, camera: any, model: any, lights: any) {
        return {
            uModelViewMatrix: camera.getViewMatrix().multiply(model.transform),
            uProjectionMatrix: camera.getProjectionMatrix(),
            uBaseColor: this.color,
            // ...其它uniform
        };
    }

    getTextures(): TextureInfo {
        return {};
    }

    // 在 Material.ts 中添加
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
                cullMode: 'none',
                frontFace: 'ccw'
            },
            depthStencil: {
                depthWriteEnabled: this.depthWrite,
                depthCompare: this.depthFunc === WebGLConstants.LEQUAL ? 'less-equal' : 'less', // 你可以做更全面的映射
                format: 'depth24plus'
            }
        };
    }

}
