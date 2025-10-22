import { Context } from '../Context';
import { Texture } from '../../textures/Texture';
import {
    getWebGPUWrapMode,
    getWebGPUMinMagFilter
} from '../../textures/TextureUtils';

import type { BufferDescriptor, WriteBufferDescriptor } from '../BufferDescriptor';
import type { TextureDescriptor } from '../TextureDescriptor';

export type WebGPUContextOptions= {

};

export class WebGPUContext extends Context {
    public device!: GPUDevice;
    public queue!: GPUQueue;
    // public canvas!: HTMLCanvasElement;
    public context!: GPUCanvasContext;
    public format!: GPUTextureFormat;

    private defaultTexture!: Texture;
    private defaultTextureView!: GPUTextureView;
    private defaultSampler!: GPUSampler;
    
    private _textureMap: WeakMap<Texture, GPUTexture> = new WeakMap();
    private _samplerMap: WeakMap<Texture, GPUSampler> = new WeakMap();

    constructor(canvas: HTMLCanvasElement, options: WebGPUContextOptions = {}) {
        super();

        // 确保传入的 canvas 元素存在
        this.canvas = canvas;
        if (!(this.canvas instanceof HTMLCanvasElement)) {
            throw new Error('Canvas element is required for WebGLContext');
        }

        // this.init(options)
    }

    get width(): number {
        return this.canvas.width;
    }

    set width(value: number) {
        this.canvas.width = value;
    }

    get height(): number {
        return this.canvas.height;
    }

    set height(value: number) {
        this.canvas.height = value;
    }

    async init(options: WebGPUContextOptions = {}) {
        // 1. 获取 WebGPU 适配器和设备
        if (!navigator.gpu) throw new Error('WebGPU not supported');
        const adapter = await navigator.gpu.requestAdapter();
        if (!adapter) throw new Error('No WebGPU adapter found');

        // 2. 创建 WebGPU 设备和队列
        this.device = await adapter.requestDevice();
        this.queue = this.device.queue;

        // 3. 创建 WebGPU 上下文
        this.context = this.canvas.getContext('webgpu') as GPUCanvasContext;
        this.format = navigator.gpu.getPreferredCanvasFormat();
        this.configureSwapChain();

        // 创建默认纹理和采样器
        this.defaultTexture = Context.createDefaultTexture();
    }

    configureSwapChain() {
        if (!this.context || !this.device) return;
        this.context.configure({
            device: this.device,
            format: this.format,
            alphaMode: 'opaque'
        });
    }

    resize(width: number, height: number): void {
        this.width = width;
        this.height = height;
        this.configureSwapChain();
    }

    getDevice(): WebGLRenderingContext | GPUDevice {
        return this.device;
    }

    clear(): void {
        const commandEncoder = this.device.createCommandEncoder();
        const texture = this.context.getCurrentTexture();
        const view = texture.createView();

        const renderPass = commandEncoder.beginRenderPass({
            colorAttachments: [{
                view,
                loadOp: 'clear',
                clearValue: { r: 0, g: 0, b: 0, a: 1 },
                storeOp: 'store'
            }]
        });

        renderPass.end();
        this.device.queue.submit([commandEncoder.finish()]);
    }

    // static createDefaultTexture() {
    //     // 使用 ImageData 构造函数创建 ImageData 对象
    //     // 创建1x1 RGBA纹理，内容为白色
    //     const whitePixel = new Uint8Array([255, 255, 255, 255]);
    //     const defaultTexture = new Texture({
    //         name: 'default-webgpu-texture',
    //         source: new ImageData(new Uint8ClampedArray(whitePixel), 1, 1)
    //     });
    //     // const gpuTexture = this.createTexture(this.defaultTexture);
    //     // if (!gpuTexture) {
    //     //     throw new Error('Failed to create default texture');
    //     // }
    //     // const defaultTextureView = gpuTexture.createView();

    //     // 创建默认采样器
        
    //     // const defaultSampler = this.device.createSampler({
    //     //     magFilter: 'linear',
    //     //     minFilter: 'linear',
    //     //     addressModeU: 'repeat',
    //     //     addressModeV: 'repeat',
    //     // });

    //     defaultTexture.magFilter = TextureMagFilter.Linear;
    //     defaultTexture.minFilter = TextureMinFilter.Linear;
    //     defaultTexture.wrapS = TextureWrapMode.ClampToEdge;
    //     defaultTexture.wrapT = TextureWrapMode.ClampToEdge;

    //     return defaultTexture;
    // }

    /**
     * 获取当前帧的 GPUTextureView
     */
    getCurrentTextureView(): GPUTextureView {
        return this.context.getCurrentTexture().createView();
    }


    // 顶点缓冲
    // 如果你只上传一次数据，用方案一（mappedAtCreation: true）。
    // 如果你需要多次写入或更灵活，用方案二（mappedAtCreation: false + this.device.queue.writeBuffer）
    // createVertexBuffer(data: ArrayBufferView): GPUBuffer {
    //     const buffer = this.device.createBuffer({
    //         label: 'vertexBuffer',
    //         size: data.byteLength,
    //         usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST,
    //         mappedAtCreation: true  // 只有在 mappedAtCreation: true 时，才能调用 getMappedRange()
    //     });

    //     //
    //     new Uint8Array(buffer.getMappedRange()).set(new Uint8Array(data.buffer));
    //     buffer.unmap();

    //     return buffer;
    // }

    createVertexBuffer(desc: BufferDescriptor): GPUBuffer {
        return this.device.createBuffer({
            label: desc.label,
            size: desc.byteLength,
            usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST,
            mappedAtCreation: desc.mappedAtCreation
        });
    }

    // bindVertexBuffer(buffer: GPUBuffer): void {
    //     // 可选：用于配合 draw 调用时绑定
    //     // 实际在 draw 阶段使用 passEncoder.setVertexBuffer()
    // }

    // 索引缓冲
    createIndexBuffer(desc: BufferDescriptor): GPUBuffer {
        return this.device.createBuffer({
            label: desc.label,
            size: desc.byteLength,
            usage: GPUBufferUsage.INDEX | GPUBufferUsage.COPY_DST,
            mappedAtCreation: desc.mappedAtCreation
        });
    }

    // bindIndexBuffer(buffer: GPUBuffer): void {
    //     // 实际在 draw 阶段使用 passEncoder.setIndexBuffer()
    // }

    createInstanceBuffer(desc: BufferDescriptor): GPUBuffer {
        return this.device.createBuffer({
            label: desc.label,
            size: desc.byteLength,
            usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST,
            mappedAtCreation: desc.mappedAtCreation
        });
    }

    createUniformBuffer(desc: BufferDescriptor): GPUBuffer {
        return this.device.createBuffer({
            label: desc.label,
            size: desc.byteLength,
            usage: GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST,
            mappedAtCreation: desc.mappedAtCreation
        });
    }

    // bindInstanceBuffer(buffer: any): void {
    //     // 实际在 draw 阶段通过 layout binding 使用
    // }

    writeBuffer(desc: WriteBufferDescriptor) {
        const { buffer, data, offset = 0, mappedAtCreation = false } = desc;
        if (mappedAtCreation) {
            new Uint8Array(buffer.getMappedRange()).set(new Uint8Array(data.buffer, offset, data.byteLength));
            buffer.unmap();
        } else {
            this.queue.writeBuffer(buffer, offset, data.buffer, data.byteOffset, data.byteLength);
        }
    }

    /**
     * 删除 GPUBuffer
     * @param buffer GPUBuffer
     */
    deleteBuffer(buffer: GPUBuffer | null | undefined): void {
        if (buffer && typeof buffer.destroy === 'function') {
            buffer.destroy();
        }
    }

    // 创建纹理
    // createTexture(texture: Texture): GPUTexture {
    //     let gpuTexture = this._textureMap.get(texture);
    //     if (gpuTexture) return gpuTexture;

    //     // 处理图片数据
    //     const width = texture.width || 1;
    //     const height = texture.height || 1;
    //     let imageData: ImageData | null = null;

    //     if (texture.image instanceof ImageData) {
    //         imageData = texture.image;
    //     } else if (!texture.image) {
    //         imageData = Texture.DefaultImageData;
    //     }

    //     gpuTexture = this.device.createTexture({
    //         size: [width, height, 1],
    //         format: 'rgba8unorm',
    //         usage: GPUTextureUsage.TEXTURE_BINDING | GPUTextureUsage.COPY_DST | GPUTextureUsage.RENDER_ATTACHMENT
    //     });

    //     // 上传像素数据
    //     if (imageData) {
    //         this.device.queue.writeTexture(
    //             { texture: gpuTexture },
    //             imageData.data,
    //             { bytesPerRow: imageData.width * 4 },
    //             { width: imageData.width, height: imageData.height, depthOrArrayLayers: 1 }
    //         );
    //     } else {
    //         // 兜底：1x1白色像素
    //         const whitePixel = new Uint8Array([255, 255, 255, 255]);
    //         this.device.queue.writeTexture(
    //             { texture: gpuTexture },
    //             whitePixel,
    //             { bytesPerRow: 4 },
    //             { width: 1, height: 1, depthOrArrayLayers: 1 }
    //         );
    //     }

    //     this._textureMap.set(texture, gpuTexture);
    //     return gpuTexture;
    // }
    createTexture(desc: TextureDescriptor): GPUTexture {
        // let gpuTexture = this._textureMap.get(texture);
        // if (gpuTexture) return gpuTexture;
    
        const width = desc.width || 1;
        const height = desc.height || 1;
        const format: GPUTextureFormat = desc.format || 'rgba8unorm';
        const usage: GPUTextureUsageFlags =
            desc.usage ||
            (GPUTextureUsage.TEXTURE_BINDING | GPUTextureUsage.COPY_DST | GPUTextureUsage.RENDER_ATTACHMENT);
    
        const gpuTexture = this.device.createTexture({
            size: [width, height, 1],
            format,
            usage,
            // 可扩展：mipLevelCount: texture.mipLevelCount, sampleCount: texture.sampleCount, dimension: texture.dimension
        });
    
        // this._textureMap.set(texture, gpuTexture);
        return gpuTexture;
    }

    writeTexture(texture: Texture) {
        let imageData: ImageData | null = null;
        let width = 1, height = 1;
    
        if (!texture.image) {
            imageData = Texture.DefaultImageData;
            width = imageData.width;
            height = imageData.height;
        } else if (texture.image instanceof ImageData) {
            imageData = texture.image;
            width = imageData.width;
            height = imageData.height;
        } else if (typeof HTMLImageElement !== "undefined" && texture.image instanceof HTMLImageElement) {
            // 如需支持 HTMLImageElement，可用 canvas 转为 ImageData
            const canvas = document.createElement('canvas');
            canvas.width = texture.image.width;
            canvas.height = texture.image.height;
            const ctx = canvas.getContext('2d');
            if (ctx) {
                ctx.drawImage(texture.image, 0, 0);
                imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
                width = imageData.width;
                height = imageData.height;
            }
        }
    
        if (imageData) {
            this.device.queue.writeTexture(
                { texture: texture.gpuTexture as GPUTexture },
                imageData.data,
                { bytesPerRow: width * 4 },
                { width, height, depthOrArrayLayers: 1 }
            );
        } else {
            // 兜底：1x1白色像素
            const whitePixel = new Uint8Array([255, 255, 255, 255]);
            this.device.queue.writeTexture(
                { texture: texture.gpuTexture as GPUTexture },
                whitePixel,
                { bytesPerRow: 4 },
                { width: 1, height: 1, depthOrArrayLayers: 1 }
            );
        }
    }

    /**
     * 删除 GPUTexture
     * @param texture GPUTexture
     */
    deleteTexture(texture: GPUTexture | null | undefined): void {
        if (texture && typeof texture.destroy === 'function') {
            texture.destroy();
        }
    }


    /**
     * 获取 GPUTextureView
     */
    // getTextureView(texture: Texture): GPUTextureView {
    //     const gpuTexture = this.createTexture(texture);
    //     return gpuTexture.createView();
    // }

    getTextureView(texture: Texture): GPUTextureView {
        // const gpuTexture = this.createTexture(texture);
        if (!texture.gpuTexture) {
            console.error('Failed to create GPUTexture for texture: ' + texture.name);
            return this.defaultTextureView;
        }
        return (texture.gpuTexture as GPUTexture).createView();
    }

    /**
     * 获取或创建采样器
     */
    getSampler(texture: Texture): GPUSampler {
        let sampler = this._samplerMap.get(texture);
        if (sampler) return sampler;

        sampler = this.device.createSampler({
            magFilter: getWebGPUMinMagFilter(texture.magFilter),
            minFilter: getWebGPUMinMagFilter(texture.minFilter),
            addressModeU: getWebGPUWrapMode(texture.wrapS),
            addressModeV: getWebGPUWrapMode(texture.wrapT),
        });
        this._samplerMap.set(texture, sampler);
        return sampler;
    }

    /**
     * 简化绑定：返回 { view, sampler }
     */
    // getBindGroupTextureEntry(texture: Texture): { view: GPUTextureView, sampler: GPUSampler } {
    //     return {
    //         view: this.getTextureView(texture),
    //         sampler: this.getSampler(texture)
    //     };
    // }

    // bindTexture(texture: Texture, bindGroup: GPUBindGroup, binding: number): void {
    //     // passEncoder.setBindGroup(binding, bindGroup);
    // }

    // ------ BindGroup 与 Pipeline ------
    createBindGroupLayout(desc: GPUBindGroupLayoutDescriptor): GPUBindGroupLayout {
        return this.device.createBindGroupLayout(desc);
    }

    createBindGroup(desc: GPUBindGroupDescriptor): GPUBindGroup {
        return this.device.createBindGroup(desc);
    }

    createRenderPipelineLayout(desc: GPUPipelineLayoutDescriptor): GPUPipelineLayout {
        return this.device.createPipelineLayout(desc);
    }

    createRenderPipeline(desc: GPURenderPipelineDescriptor): GPURenderPipeline {
        return this.device.createRenderPipeline(desc);
    }

    createComputePipeline(desc: GPUComputePipelineDescriptor): GPUComputePipeline {
        return this.device.createComputePipeline(desc);
    }

    // ------ Shader/Command ------
    createShaderModule(desc: GPUShaderModuleDescriptor): GPUShaderModule {
        return this.device.createShaderModule(desc);
    }

    createCommandEncoder(desc?: GPUCommandEncoderDescriptor): GPUCommandEncoder {
        return this.device.createCommandEncoder(desc);
    }

    // 绘制
    // draw(component: Model, passEncoder: GPURenderPassEncoder) {
    //     // const vertexBuffer = model.vertexBuffer;
    //     // const indexBuffer = model.indexBuffer;

    //     // if (vertexBuffer) {
    //     //     passEncoder.setVertexBuffer(0, vertexBuffer as GPUBuffer);
    //     // }

    //     // if (indexBuffer) {
    //     //     passEncoder.setIndexBuffer(indexBuffer as GPUBuffer, 'uint16');
    //     // }

    //     // // 绑定材质、着色器参数等...
    //     // // passEncoder.setBindGroup(...)

    //     // if (model.drawIndexed && indexBuffer) {
    //     //     passEncoder.drawIndexed(model.indexCount);
    //     // } else {
    //     //     passEncoder.draw(model.vertexCount);
    //     // }
    // }
    
}