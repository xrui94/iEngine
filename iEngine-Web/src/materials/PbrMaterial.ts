import { Material } from './Material';
import { Color } from './Color';
import { Texture } from '../textures/Texture';
import { DirectionalLight } from '../lights/DirectionalLight';
import { WebGLConstants } from '../renderers/webgl/WebGLConstants';
import { Context } from '../renderers/Context';
import { Matrix3 } from '../math/Matrix3';
import { Matrix4 } from '../math/Matrix4';
import type { Renderable } from '../renderers/Renderable';

import type { TextureInfo } from './Material';
import type { Camera } from '../views/cameras/Camera';
import type { Light } from '../lights/Light';
import type { Mesh } from '../core/Mesh';
import type { WebGLRenderPipelineState } from '../renderers/webgl/WebGLRenderPipeline';
import type { WebGPUShaderModule } from '../renderers/webgpu/WebGPUShaderModule';


export interface PbrMaterialParams {
    name?: string;
    shaderName: string;
    baseColor?: Color;
    baseColorMap?: Texture | string;
    // 金属光泽度粗糙度
    metallic?: number;
    roughness?: number;
    metallicRoughnessMap?: Texture;
    // 法线贴图
    normalMap?: Texture;
    normalScale?: number;
    // 环境遮蔽贴图
    aoMap?: Texture;
    aoStrength?: number;
    // 自发光
    emissiveColor?: Color;
    emissiveIntensity?: number;
    emissiveMap?: Texture;
}

export class PbrMaterial extends Material {
    // 基础颜色（albedo/baseColor）
    baseColor: Color = Color.white;
    // albedo: Color = Color.white;
    baseColorMap?: Texture | string;

    metallic: number = 0.0;
    roughness: number = 1.0;
    metallicRoughnessMap?: Texture | string;

    normalMap?: Texture | string;
    normalScale: number = 1.0;

    aoMap?: Texture | string;
    aoStrength: number = 1.0;

    emissiveColor: Color = Color.white;
    emissiveIntensity: number = 1.0;
    emissiveMap?: Texture | string;

    _needsUpdate: boolean = false;

    //
    // depthTest: boolean = true;
    // depthWrite: boolean = true;
    // depthFunc: number = WebGLConstants.LEQUAL;
    // transparent: boolean = true;
    // doubleSided: boolean = true;

    constructor(params: PbrMaterialParams = { shaderName: 'base_pbr' }) {
        super(
            'pbr',
            params.shaderName ?? 'base_pbr',
            params
        );

        if (params.baseColor !== undefined) this.baseColor = params.baseColor;
        if (params.baseColorMap) {
            if (params.baseColorMap instanceof Texture) {
                this.baseColorMap = params.baseColorMap;
            }
            else if (typeof params.baseColorMap === 'string') {
                this.baseColorMap = new Texture({
                    name: 'baseColorMap',
                    source: params.baseColorMap
                });
            }
        }  else {
            this.baseColorMap = Context.createDefaultTexture('baseColor');
        }

        if (params.metallic !== undefined) this.metallic = params.metallic;
        if (params.roughness !== undefined) this.roughness = params.roughness;
        if (params.metallicRoughnessMap) {
            if (params.metallicRoughnessMap instanceof Texture) {
                this.metallicRoughnessMap = params.metallicRoughnessMap;
            }
            else if (typeof params.metallicRoughnessMap === 'string') {
                this.metallicRoughnessMap = new Texture({
                    name: 'metallicRoughnessMap',
                    source: params.metallicRoughnessMap });
            } 
        } else {
            this.metallicRoughnessMap = Context.createDefaultTexture('metallicRoughness');
        }

        if (params.normalMap) {
            if (params.normalMap instanceof Texture) {
                this.normalMap = params.normalMap;
            }
            else if (typeof params.normalMap === 'string') {
                this.normalMap = new Texture({
                    name: 'normalMap',
                    source: params.normalMap
                });
            } 
        } else {
            this.normalMap = Context.createDefaultTexture('normal');
            // 默认不扰动法线，因为默认的法线贴图是1x1的，没有细节。 
            // 如果进行扰动，会覆盖掉顶点法线（如果有顶点法线）带来的细节。 
            this.normalScale = 0.0;
        }

        if (params.normalScale !== undefined) this.normalScale = params.normalScale;
        if (params.aoMap) {
            if (params.aoMap instanceof Texture) {
                this.aoMap = params.aoMap;
            }
            else if (typeof params.aoMap === 'string') {
                this.aoMap = new Texture({
                    name: 'aoMap',
                    source: params.aoMap
                });
            } 
        } else {
            this.aoMap = Context.createDefaultTexture('ao');
        }

        if (params.aoStrength !== undefined) this.aoStrength = params.aoStrength;
        if (params.emissiveColor !== undefined) this.emissiveColor = params.emissiveColor;
        if (params.emissiveIntensity !== undefined) this.emissiveIntensity = params.emissiveIntensity;
        if (params.emissiveMap) {
            if (params.emissiveMap instanceof Texture) {
                this.emissiveMap = params.emissiveMap;
            }
            else if (typeof params.emissiveMap === 'string') {
                this.emissiveMap = new Texture({
                    name: 'emissiveMap',
                    source: params.emissiveMap
                });
            } 
        } else {
            this.emissiveMap = Context.createDefaultTexture('emissive');
        }
    }

    needsUpdate(): boolean {
        return this._needsUpdate;
    }

    markUpdated(): void {
        this._needsUpdate = false;
    }
    
    getShaderMacroDefines(): Record<string, string | number | boolean> {
        const defines: Record<string, boolean> = {};
        if (this.baseColorMap) defines.HAS_BASECOLORMAP = true;
        if (this.normalMap) defines.HAS_NORMALMAP = true;
        if (this.metallicRoughnessMap) defines.HAS_METALLICROUGHNESSMAP = true;
        if (this.aoMap) defines.HAS_AOMAP = true;
        if (this.emissiveMap) defines.HAS_EMISSIVEMAP = true;
        return defines;
    }

    getRenderPipelineState(): WebGLRenderPipelineState {
        return {
            depthTest: this.depthTest ?? true,
            depthWrite: this.depthWrite ?? true,
            depthFunc: this.depthFunc ?? WebGLConstants.LEQUAL,
            blend: this.transparent ?? false,
            blendFunc: this.transparent
                ? { srcRGB: WebGLConstants.SRC_ALPHA, dstRGB: WebGLConstants.ONE_MINUS_SRC_ALPHA, srcAlpha: WebGLConstants.ONE, dstAlpha: WebGLConstants.ZERO }
                : undefined,
            cullFace: this.doubleSided ? false : true,
            cullMode: WebGLConstants.BACK,
            // ...其它状态按需补充
        };
    }

    // // 在 PbrMaterial.ts 中添加
    // getWebGPURenderPipelineDescriptor(device: GPUDevice, shader: WebGPUShaderModule, mesh: Mesh): GPURenderPipelineDescriptor {
    //     const bindGroupLayout = device.createBindGroupLayout({
    //         entries: [
    //             { binding: 0, visibility: GPUShaderStage.VERTEX | GPUShaderStage.FRAGMENT, buffer: { type: 'uniform' } },
    //             // uBaseColorMap
    //             { binding: 1, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: 'float' } },
    //             { binding: 2, visibility: GPUShaderStage.FRAGMENT, sampler: { type: 'filtering' } },
    //             // uMetallicRoughnessMap
    //             { binding: 3, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: 'float' } },
    //             { binding: 4, visibility: GPUShaderStage.FRAGMENT, sampler: { type: 'filtering' } },
    //             // uNormalMap
    //             { binding: 5, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: 'float' } },
    //             { binding: 6, visibility: GPUShaderStage.FRAGMENT, sampler: { type: 'filtering' } },
    //             // uAoMap
    //             { binding: 7, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: 'float' } },
    //             { binding: 8, visibility: GPUShaderStage.FRAGMENT, sampler: { type: 'filtering' } },
    //             // uEmissiveMap
    //             { binding: 9, visibility: GPUShaderStage.FRAGMENT, texture: { sampleType: 'float' } },
    //             { binding: 10, visibility: GPUShaderStage.FRAGMENT, sampler: { type: 'filtering' } },
    //             // ...依次添加所有贴图和采样器
    //         ]
    //     });
    //     return {
    //         label: 'pbrRenderPipeline',
    //         // layout: 'auto',
    //         layout: device.createPipelineLayout({ bindGroupLayouts: [bindGroupLayout] }),
    //         vertex: {
    //             module: shader.getModule(),
    //             entryPoint: 'vs_main',
    //             buffers: [mesh.getVertexLayout()] as Iterable<GPUVertexBufferLayout>, // 你需要实现此方法
    //         },
    //         fragment: {
    //             module: shader.getModule(),
    //             entryPoint: 'fs_main',
    //             targets: [{
    //                 format: 'bgra8unorm', // 或根据 context.format
    //                 blend: this.transparent ? {
    //                     color: {
    //                         srcFactor: 'src-alpha',
    //                         dstFactor: 'one-minus-src-alpha',
    //                         operation: 'add'
    //                     },
    //                     alpha: {
    //                         srcFactor: 'one',
    //                         dstFactor: 'zero',
    //                         operation: 'add'
    //                     }
    //                 } : undefined
    //             }]
    //         },
    //         primitive: {
    //             topology: mesh.primitive.getGPUPrimitiveType(),
    //             cullMode: this.doubleSided ? 'none' : 'back',
    //             frontFace: 'ccw'
    //         },
    //         depthStencil: {
    //             depthWriteEnabled: this.depthWrite,
    //             depthCompare: this.depthFunc === WebGLConstants.LEQUAL ? 'less-equal' : 'less', // 你可以做更全面的映射
    //             format: 'depth24plus'
    //         }
    //     };
    // }

    getUniforms(context: Context, camera: Camera, component: Renderable, lights: Light[]) {
        // 只取第一个环境光和第一个方向光
        let ambientColor = [0, 0, 0];
        let dirLightDir = [0, -1, 0];
        let dirLightColor = [1, 1, 1];
        let dirLightIntensity = 10.0;

        for (const light of lights || []) {
            if (light.type === 'ambient' && ambientColor[0] === 0 && ambientColor[1] === 0 && ambientColor[2] === 0) {
                ambientColor = light.color.toArray();
            }
            if (light.type === 'directional') {
                dirLightDir = (light as DirectionalLight).direction.toArray();
                dirLightColor = (light as DirectionalLight).color.toArray();
                dirLightIntensity = (light as DirectionalLight).intensity;
                break; // 只取第一个方向光
            }
        }

        // 计算法线矩阵
        const normalMatrix = component.worldTransform.inverse().transpose().toMatrix3();

        // 贴图相关 uniform
        const uniforms: Record<string, any> = {
            uModelViewMatrix: camera.getViewMatrix().multiply(component.worldTransform),
            uProjectionMatrix: camera.getProjectionMatrix(),
            uNormalMatrix: normalMatrix,

            // 相机位置
            uCameraPos: camera.position.toArray(),

            // PBR材质相关参数
            uBaseColor: this.baseColor,
            uMetallic: this.metallic,
            uRoughness: this.roughness,

            // 法线贴图
            uNormalScale: this.normalScale,

            // 环境遮蔽
            uAoStrength: this.aoStrength,

            // 自发光
            uEmissiveColor: this.emissiveColor,
            uEmissiveIntensity: this.emissiveIntensity,

            // 环境光
            uAmbientColor: ambientColor,

            // 方向光
            uLightDir: dirLightDir,
            uLightColor: dirLightColor,
            uLightIntensity: dirLightIntensity,
        };

        // // 贴图（如有则传递）
        // if (this.baseColorMap) uniforms.uBaseColorMap = this.baseColorMap;
        // if (this.metallicRoughnessMap) uniforms.uMetallicRoughnessMap = this.metallicRoughnessMap;
        // if (this.normalMap) uniforms.uNormalMap = this.normalMap;
        // if (this.aoMap) uniforms.uAoMap = this.aoMap;
        // if (this.emissiveMap) uniforms.uEmissiveMap = this.emissiveMap;

        return uniforms;
    }

    getTextures(): TextureInfo {
        // 贴图（如有则传递）
        const textures: TextureInfo = {};
        // if (this.baseColorMap) textures.uBaseColorMap = this.baseColorMap;
        // if (this.metallicRoughnessMap) textures.uMetallicRoughnessMap = this.metallicRoughnessMap;
        // if (this.normalMap) textures.uNormalMap = this.normalMap;
        // if (this.aoMap) textures.uAoMap = this.aoMap;
        // if (this.emissiveMap) textures.uEmissiveMap = this.emissiveMap;

        textures.uBaseColorMap = this.baseColorMap as Texture;
        textures.uMetallicRoughnessMap = this.metallicRoughnessMap as Texture;
        textures.uNormalMap = this.normalMap as Texture;
        textures.uAoMap = this.aoMap as Texture;
        textures.uEmissiveMap = this.emissiveMap as Texture;
        
        return textures;
    }

}
