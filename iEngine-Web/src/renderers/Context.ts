import {
    Texture,
    TextureMagFilter,
    TextureMinFilter,
    TextureWrapMode,
    TextureKind,
    DefaultTexturePixels
} from '../textures/Texture';
import { BufferDescriptor, WriteBufferDescriptor } from './BufferDescriptor';
import { TextureDescriptor } from './TextureDescriptor';


export abstract class Context {
    protected _canvas!: HTMLCanvasElement;

    abstract get width(): number;
    abstract set width(value: number);

    abstract get height(): number;
    abstract set height(value: number);

    constructor(canvas: HTMLCanvasElement) {
        this._canvas = canvas;
        if (!(this._canvas instanceof HTMLCanvasElement)) {
            throw new Error('Canvas element is required for WebGLContext / WebGPUContext');
        }
    }

    /**
     * 获取显示尺寸的宽度，这是指 canvas 在浏览器中的实际显示宽度，相当于clientWidth
     * @returns
     */
    get displayWidth(): number {
        return this._canvas.clientWidth;
    }

    /**
     * 获取显示尺寸的高度，这是指 canvas 在浏览器中的实际显示的高度，相当于clientHeight
     * @returns 
     */
    get displayHeight(): number {
        return this._canvas.clientHeight;
    }

    abstract getDevice(): WebGLRenderingContext | GPUDevice;
    abstract clear(): void;
    abstract resize(width: number, height: number): void;


    // abstract createVertexBuffer(data: ArrayBufferView): any;
    // abstract bindVertexBuffer(buffer: any): void;

    // abstract createIndexBuffer(data: ArrayBufferView): any;
    // abstract bindIndexBuffer(buffer: any): void;

    // abstract createInstanceBuffer(data: ArrayBufferView): any;
    // abstract bindInstanceBuffer(buffer: any): void;

    // 统一 buffer 创建接口（byteLength 必填，其它参数后端自用/可选）
    abstract createVertexBuffer(desc?: BufferDescriptor): WebGLBuffer | GPUBuffer;
    abstract createIndexBuffer(desc?: BufferDescriptor): WebGLBuffer | GPUBuffer;
    abstract createInstanceBuffer(desc?: BufferDescriptor): WebGLBuffer | GPUBuffer;
    abstract createUniformBuffer(desc?: BufferDescriptor): WebGLBuffer | GPUBuffer;

    // 统一 buffer 写入接口
    abstract writeBuffer(desc: WriteBufferDescriptor): void;
    abstract deleteBuffer(buffer: any): void;   // 基类用 any，子类收窄类型，是 TypeScript 多后端抽象的标准做法。

    // 纹理相关接口（可选，建议抽象）
    abstract createTexture(desc?: TextureDescriptor): WebGLTexture | GPUTexture;
    abstract writeTexture(texture: Texture): void;   // 基类用 any，子类收窄类型，是 TypeScript 多后端抽象的标准做法。
    abstract deleteTexture(texture: any): void;   // 基类用 any，子类收窄类型，是 TypeScript 多后端抽象的标准做法。
    
    // 可扩展更多通用接口

    // static createDefaultTexture(kind: TextureKind = "white", pixelData?: Uint8Array) {
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

    static createDefaultTexture(
        kind: TextureKind = "white",
        pixelData?: Uint8Array,
        width: number = 1,
        height: number = 1
    ) {
        let pixel: Uint8Array | undefined;
        let name = `default-${kind}-texture`;

        if (kind === "custom") {
            if (!pixelData) {
                throw new Error("For 'custom' TextureKind, pixelData must be provided.");
            }
            if (pixelData.length !== width * height * 4) {
                throw new Error(
                    `pixelData length (${pixelData.length}) does not match width*height*4 (${width * height * 4})`
                );
            }
            pixel = pixelData;
            name = "custom-default-texture";
        } else {
            // 当kind = "custom"时，这行理论上不会被执行（前面已单独处理 "custom"）
            // 把 kind 断言为不是 "custom" 的类型（因为 "custom" 不在 DefaultTexturePixels 里）
            pixel = DefaultTexturePixels[kind as Exclude<TextureKind, "custom">] || DefaultTexturePixels["white"];
            // 允许 pixelData 覆盖默认（但也要校验）
            if (pixelData) {
                if (pixelData.length !== width * height * 4) {
                    throw new Error(
                        `pixelData length (${pixelData.length}) does not match width*height*4 (${width * height * 4})`
                    );
                }
                pixel = pixelData;
            }
        }

        const texture = new Texture({
            name,
            source: new ImageData(new Uint8ClampedArray(pixel), width, height)
        });

        texture.magFilter = TextureMagFilter.Linear;
        texture.minFilter = TextureMinFilter.Linear;
        texture.wrapS = TextureWrapMode.ClampToEdge;
        texture.wrapT = TextureWrapMode.ClampToEdge;

        return texture;
    }
}