import { Renderer } from '../Renderer';
import { Camera } from '../../views/cameras/Camera';
import { PerspectiveCamera } from '../../views/cameras/PerspectiveCamera';
import { Scene } from '../../scenes/Scene';
import { Texture } from '../../textures/Texture';
import { WebGPUContext } from './WebGPUContext';
import { ShaderLib, BaseShaders, makeShaderVariantKey } from '../../shaders/ShaderLib';
import { WebGPUUniforms } from './WebGPUUniforms';
import { WebGPUShaderModule, createPipelineKey } from './WebGPUShaderModule';
import { Light } from '../../lights/Light';
import { Renderable } from '../Renderable';

import type { GraphicsAPI } from '../Renderer';
import type { WebGPUContextOptions } from './WebGPUContext'
import type { Mesh } from '../../core/Mesh';

function uniformsToFloat32Array(uniforms: Record<string, any>): Float32Array {
    // 假设顺序为: uModelViewMatrix, uProjectionMatrix, uNormalMatrix, uCameraPos, uBaseColor, ...
    // 你可以根据实际 WGSL uniform struct 顺序来组织
    const arr: number[] = [];
    if (uniforms.uModelViewMatrix) arr.push(...uniforms.uModelViewMatrix.elements);
    if (uniforms.uProjectionMatrix) arr.push(...uniforms.uProjectionMatrix.elements);
    if (uniforms.uNormalMatrix) {
        arr.push(...uniforms.uNormalMatrix.elements);
        // const m = uniforms.uNormalMatrix.elements;
        // if (m.length === 9) {
        //     arr.push(m[0], m[1], m[2], 0);
        //     arr.push(m[3], m[4], m[5], 0);
        //     arr.push(m[6], m[7], m[8], 0);
        // } else {
        //     // 其它情况按实际补齐
        //     arr.push(...m);
        // }
    }
    if (uniforms.uCameraPos) {
        arr.push(...uniforms.uCameraPos);
        if (uniforms.uCameraPos.length === 3) arr.push(0); // padding，补齐4 float
    }
    if (uniforms.uBaseColor) {
        arr.push(...(uniforms.uBaseColor.toArray ? uniforms.uBaseColor.toArray() : uniforms.uBaseColor));
        if (uniforms.uBaseColor.length === 3) arr.push(0); // padding，补齐4 float
    }
    if ('uMetallic' in uniforms) arr.push(uniforms.uMetallic ?? 0);
    if ('uRoughness' in uniforms) arr.push(uniforms.uRoughness ?? 1);
    if (uniforms.uEmissive) {
        arr.push(...(uniforms.uEmissive.toArray ? uniforms.uEmissive.toArray() : uniforms.uEmissive));
        if (uniforms.uEmissive.length === 3) arr.push(0); // padding，补齐4 float
    }
    if ('uAoStrength' in uniforms) arr.push(uniforms.uAoStrength ?? 1);
    if ('uNormalScale' in uniforms) arr.push(uniforms.uNormalScale ?? 1);
    if (uniforms.uAmbientColor) {
        arr.push(...(uniforms.uAmbientColor.toArray ? uniforms.uAmbientColor.toArray() : uniforms.uAmbientColor));
        if (uniforms.uAmbientColor.length === 3) arr.push(0); // padding，补齐4 float
    }
    if (uniforms.uLightDir) {
        arr.push(...(uniforms.uLightDir.toArray ? uniforms.uLightDir.toArray() : uniforms.uLightDir));
        if (uniforms.uLightDir.length === 3) arr.push(0); // padding，补齐4 float
    }
    if (uniforms.uLightColor) {
        arr.push(...(uniforms.uLightColor.toArray ? uniforms.uLightColor.toArray() : uniforms.uLightColor));
        if (uniforms.uLightColor.length === 3) arr.push(0); // padding，补齐4 float
    }
    if ('uLightIntensity' in uniforms) arr.push(uniforms.uLightIntensity ?? 1);
    // ...如有其它uniform，继续判断
    // 最后补齐到 16 字节倍数
    while (arr.length % 4 !== 0) {
        arr.push(0);
    }

    return new Float32Array(arr);
}

export class WebGPURenderer extends Renderer {
    private static instanceCount = 0;
    private _canvas: HTMLCanvasElement = null!;
    private _context!: WebGPUContext;
    private _clearColor: [number, number, number, number] = [0.1, 0.1, 0.1, 1];
    private _viewport: { x: number; y: number; width: number; height: number } | null = null;
    private graphicsApi: GraphicsAPI = 'webgpu';
    private shaders: Map<string, WebGPUShaderModule> = new Map();
    private _isInitialized: boolean = false;
    private currentCamera: Camera | null = null;

    // private defaultTexture!: Texture;
    // private defaultTextureView: GPUTextureView | null = null;
    // private defaultSampler: GPUSampler | null = null;

    private device: GPUDevice | null = null;
    // private context: GPUCanvasContext;
    // private format: GPUTextureFormat;
    private commandEncoder: GPUCommandEncoder | null = null;
    private renderPipelineCache: Map<string, GPURenderPipeline> = new Map();

    constructor(/*canvas: HTMLCanvasElement, options: WebGPUContextOptions = {}*/) {
        super();

        // this.context = new WebGPUContext(canvas, options);
        // this.initShaders();
        // this.resize();

        WebGPURenderer.instanceCount++;
        if (WebGPURenderer.instanceCount > 1) {
            console.warn(
                'Multiple WebGPURenderer instances detected. ' +
                'This is usually unnecessary and may impact performance. ' +
                'Consider reusing a single instance.'
            );
        }
    }

    // 提供gpu上下文的访问器
    get gpuContext(): GPUDevice {
        return (this._context.getDevice()) as GPUDevice;
    }

    async init(canvas: HTMLCanvasElement, options: WebGPUContextOptions) {
        this._canvas = canvas;
        this._context = new WebGPUContext(canvas, options);
        await this._context.init();

        //
        // this.initShaders();
        this.device = this.gpuContext;
        this.resize();

        // 创建默认纹理和采样器
        // this.createDefaultTextureAndSampler(this.gpuContext);
        this._isInitialized = true;
    }

    isInitialized(): boolean {
        return this._isInitialized;
    }

    // private initShaders(): void {
    //     // 提前编译所有基本的着色器

    //     if (!Renderer.isGraphicsAPI(this.graphicsApi)) {
    //         throw 'Failed to get Graphic API.';
    //     }

    //     for (const shaderName of BaseShaders) {
    //         const variant = ShaderLib.getVariant(shaderName, this.graphicsApi);
    //         if (variant && variant.wgsl) {
    //             const shader = new WebGPUShaderModule(
    //                 this.gpuContext, variant.wgsl
    //             );
    //             this.shaders.set(shaderName, shader);
    //         }
    //     }        
    // }

    // getOrCreateShader(
    //     shaderName: string,
    //     defines: Record<string, string | number | boolean>
    // ): WebGPUShaderModule | null {
    //     const key = makeShaderVariantKey(shaderName, defines);
    //     let shader = this.shaders.get(key);
    //     if (!shader) {
    //         const variant = ShaderLib.getVariant(shaderName, this.graphicsApi, defines);
    //         if (!variant || !variant.wgsl) {
    //             console.warn(`Shader variant not found for "${shaderName}"`);
    //             return null;
    //         }
    //         shader = new WebGPUShaderModule(
    //             this.gpuContext, variant.wgsl
    //         );
    //         this.shaders.set(key, shader);
    //     }
    //     return shader;
    // }

    // private createDefaultTextureAndSampler(device: GPUDevice) {
    //     // 创建1x1 RGBA纹理，内容为白色
    //     // const texture = device.createTexture({
    //     //     size: [1, 1, 1],
    //     //     format: 'rgba8unorm',
    //     //     usage: GPUTextureUsage.TEXTURE_BINDING | GPUTextureUsage.COPY_DST,
    //     // });

    //     // // 上传像素数据
    //     // const whitePixel = new Uint8Array([255, 255, 255, 255]);
    //     // device.queue.writeTexture(
    //     //     { texture: texture },
    //     //     whitePixel,
    //     //     { bytesPerRow: 4, rowsPerImage: 1 },
    //     //     { width: 1, height: 1, depthOrArrayLayers: 1 }
    //     // );
    //     // this.defaultTextureView = texture.createView();

    //     // ---- 第1次修改 ---
    //     // // 使用 ImageData 构造函数创建 ImageData 对象
    //     // // 创建1x1 RGBA纹理，内容为白色
    //     // const whitePixel = new Uint8Array([255, 255, 255, 255]);
    //     // const texture = new Texture({
    //     //     name: 'default-webgpu-texture',
    //     //     source: new ImageData(new Uint8ClampedArray(whitePixel), 1, 1)
    //     // });
    //     // const gpuTexture = this.context.createTexture(texture);
    //     // if (gpuTexture) {
    //     //     this.defaultTextureView = gpuTexture.createView();
    //     // }

    //     // // 创建默认采样器
    //     // this.defaultSampler = device.createSampler({
    //     //     magFilter: 'linear',
    //     //     minFilter: 'linear',
    //     //     addressModeU: 'repeat',
    //     //     addressModeV: 'repeat',
    //     // });

    //     // --- 第2次修改 ---
    //     // 使用 ImageData 构造函数创建 ImageData 对象
    //     // 创建1x1 RGBA纹理，内容为白色
    //     const whitePixel = new Uint8Array([255, 255, 255, 255]);
    //     this.defaultTexture = new Texture({
    //         name: 'default-webgpu-texture',
    //         source: new ImageData(new Uint8ClampedArray(whitePixel), 1, 1)
    //     });
    //     // const gpuTexture = this.context.createTexture(texture);
    //     // if (gpuTexture) {
    //     //     this.defaultTextureView = gpuTexture.createView();
    //     // }

    //     // // 创建默认采样器
    //     // this.defaultSampler = device.createSampler({
    //     //     magFilter: 'linear',
    //     //     minFilter: 'linear',
    //     //     addressModeU: 'repeat',
    //     //     addressModeV: 'repeat',
    //     // });
    // }

    getOrCreatePipeline(mesh: Mesh, shader: WebGPUShaderModule, pipelineDesc?: GPURenderPipelineDescriptor): GPURenderPipeline | null {
        const key = JSON.stringify(mesh.getVertexLayout()) + '_' + shader + '_' + JSON.stringify(pipelineDesc);
        let pipeline = this.renderPipelineCache.get(key);
        if (!pipeline && pipelineDesc) {
            pipeline = this.gpuContext.createRenderPipeline(pipelineDesc);
            this.renderPipelineCache.set(key, pipeline);
        }
        return pipeline ?? null;
    }

    setClearColor(color?: [number, number, number, number]): void {
        if (color) {
            this._clearColor = color;
        }
    }

    setViewport(viewport?: { x: number; y: number; width: number; height: number }): void {
        this._viewport = viewport ?? null;
    }

    clear(): void {
        this._context.clear();
    }

    resize(): void {
        const displayWidth = this._context.displayWidth;
        const displayHeight = this._context.displayHeight;
        this._context.resize(displayWidth, displayHeight);
        
        // 更新相机宽高比（如果是透视相机）
        if (this.currentCamera instanceof PerspectiveCamera) {
            this.currentCamera.setAspect(this._context.width / this._context.height);
        }
    }

    async render(scene: Scene, options?: { clearCanvas?: boolean }): Promise<void> {
        this.currentCamera = scene.activeCamera;
        if (!this.currentCamera) {
            console.error("No active camera set for rendering");
            return;
        }

        // 使用基类统一聚合（去重）
        const final = this.aggregateRenderables(scene);

        // 检查是否有可渲染对象
        if (final.length === 0) {
            console.warn("No components to render in the scene");
            return;
        }

        // 使用基类统一聚合光源（去重）
        const finalLights = this.aggregateLights(scene);

        // 创建深度纹理（可缓存）

        const device = this.gpuContext;
        const textureView = this._context.getCurrentTextureView();
        const depthTexture = device.createTexture({
            size: [this._context.width, this._context.height, 1],
            format: 'depth24plus',
            usage: GPUTextureUsage.RENDER_ATTACHMENT
        });
        const depthView = depthTexture.createView()

        const commandEncoder = device.createCommandEncoder({
            label: 'commandEncoder'
        });
        const renderPass = commandEncoder.beginRenderPass({
            label: 'renderPass',
            colorAttachments: [{
                view: textureView,
                loadOp: "clear",
                storeOp: "store",
                clearValue: {
                    r: this._clearColor[0],
                    g: this._clearColor[1],
                    b: this._clearColor[2],
                    a: this._clearColor[3]
                }
            }],
            depthStencilAttachment: {
                view: depthView,
                depthLoadOp: 'clear',
                depthClearValue: 1.0,
                depthStoreOp: 'store',
            }
        });

        // 设置视口
        if (this._viewport) {
            const { x, y, width, height } = this._viewport;
            renderPass.setViewport(x, y, width, height, 0, 1);
        } else {
            // 默认全屏
            renderPass.setViewport(0, 0, this._context.width, this._context.height, 0, 1);
        }

        for (const component of final) {
            if (!component || !component.mesh) continue;
            if (!component.mesh.uploaded) {
                component.mesh.upload(this._context);
            }

            const defines = {
                ...component.mesh.getShaderMacroDefines(),
                ...component.material.getShaderMacroDefines()
            };
            const shader = component.material.getShader(this._context, this.graphicsApi, {
                overrideDefaultDefines: false,
            });
            if (!shader) {
                console.error('Failed get shader.');
                continue;
            }
            const pipelineDesc = (shader as WebGPUShaderModule).createPipelineDescriptor(component.mesh, component.material);
            const pipeline = (shader as WebGPUShaderModule).getRenderPipeline(
                createPipelineKey({
                    shaderName: "base_pbr",
                    defines: defines,
                    vertexLayout: 'PNCT',
                    renderTargetFormat: "bgra8unorm",
                    transparent: component.material.transparent,
                    doubleSided: component.material.doubleSided,
                    depthCompare: 'less',
                    depthWrite: component.material.depthWrite,
                    frontFace: "ccw"
                }),
                pipelineDesc
            );

            const uniforms = component.material.getUniforms(
                this._context, this.currentCamera, component, finalLights
            );

            const textures = component.material.getTextures();
            (shader as WebGPUShaderModule).setTextures(textures);            
            shader.setUniforms(uniforms);
            const bindGroup = (shader as WebGPUShaderModule).getBindGroup();
            
            renderPass.setPipeline(pipeline);
            renderPass.setBindGroup(0, bindGroup);
            renderPass.setVertexBuffer(0, (component.mesh.vbo as GPUBuffer));
            if (component.mesh.geometry.indices) {
                renderPass.setIndexBuffer((component.mesh.ibo as GPUBuffer),
                    component.mesh.geometry.indices instanceof Uint16Array ? 'uint16' : 'uint32'
                );
                renderPass.drawIndexed(component.mesh.geometry.indexCount);
            } else {
                renderPass.draw(component.mesh.geometry.vertexCount);
            }
        }
        renderPass.end();
        device.queue.submit([commandEncoder.finish()]);

    }
   
}