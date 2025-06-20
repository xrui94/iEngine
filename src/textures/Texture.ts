import type { WebGLContext } from "../renderers/webgl/WebGLContext";
import type { WebGPUContext } from "../renderers/webgpu/WebGPUContext";
import type { Context } from "../renderers/Context";

// ---- 工具函数：判断是否跨域 ----
function isCrossOrigin(url: string): boolean {
    try {
        const pageLoc = window.location;
        const urlObj = new URL(url, pageLoc.href);
        return urlObj.origin !== pageLoc.origin;
    } catch {
        return false;
    }
}

export type TextureKind =
    // PBR
    | "baseColor"
    | "metallicRoughness"
    | "normal"
    | "ao"
    | "emissive"
    | "clearcoat"
    | "sheen"
    | "transmission"
    | "iridescence"
    | "anisotropy"
    // Blinn-Phong / 传统
    | "diffuse"
    | "specular"
    | "glossiness"
    | "opacity"
    | "reflection"
    | "bump"
    | "displacement"
    | "ambient"
    | "light"
    // Lambert
    | "albedo"
    // 卡通/Toon
    | "toonRamp"
    | "toonOutline"
    // 通用
    | "white"
    | "black"
    // 用户自定义
    | "custom";

export const DefaultTexturePixels: Record<Exclude<TextureKind, "custom">, Uint8Array> = {
    baseColor: new Uint8Array([255, 255, 255, 255]),
    diffuse: new Uint8Array([255, 255, 255, 255]),
    albedo: new Uint8Array([255, 255, 255, 255]),
    metallicRoughness: new Uint8Array([0, 128, 0, 255]),
    normal: new Uint8Array([128, 128, 255, 255]),
    bump: new Uint8Array([128, 128, 255, 255]),
    ao: new Uint8Array([255, 255, 255, 255]),
    ambient: new Uint8Array([255, 255, 255, 255]),
    emissive: new Uint8Array([0, 0, 0, 255]),
    clearcoat: new Uint8Array([0, 0, 0, 255]),
    sheen: new Uint8Array([0, 0, 0, 255]),
    transmission: new Uint8Array([0, 0, 0, 255]),
    iridescence: new Uint8Array([0, 0, 0, 255]),
    anisotropy: new Uint8Array([128, 128, 255, 255]),
    specular: new Uint8Array([255, 255, 255, 255]),
    glossiness: new Uint8Array([255, 255, 255, 255]),
    opacity: new Uint8Array([255, 255, 255, 255]),
    reflection: new Uint8Array([0, 0, 0, 255]),
    displacement: new Uint8Array([128, 128, 255, 255]),
    light: new Uint8Array([255, 255, 255, 255]),
    toonRamp: new Uint8Array([255, 255, 255, 255]),
    toonOutline: new Uint8Array([0, 0, 0, 255]),
    white: new Uint8Array([255, 255, 255, 255]),
    black: new Uint8Array([0, 0, 0, 255]),
};

export enum TextureWrapMode {
    Repeat,
    ClampToEdge,
    MirroredRepeat
}

export enum TextureMinFilter {
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
}

export enum TextureMagFilter {
    Nearest,
    Linear
}

export interface TextureOptions {
    name: string;
    wrapS?: TextureWrapMode;
    wrapT?: TextureWrapMode;
    minFilter?: TextureMinFilter;
    magFilter?: TextureMagFilter;
    source?: HTMLImageElement | ImageData | HTMLCanvasElement | HTMLVideoElement | string | null;
}

export class Texture {
    public readonly name: string;
    public /*readonly*/ width: number = 2;  // 默认的占位贴图为2*2的尺寸
    public /*readonly*/ height: number = 2;
    public unit: number = 0;
    // private _source: ImageBitmap | ImageData | HTMLImageElement | HTMLCanvasElement | HTMLVideoElement | null = null;
    private _image: ImageBitmap | ImageData | HTMLImageElement | HTMLCanvasElement | HTMLVideoElement | null = null;
    private _gpuTexture: WebGLTexture | GPUTexture | null = null;
    private _gpuTextureWidth: number = 0;
    private _gpuTextureHeight: number = 0;
    private _wrapS: TextureWrapMode;
    private _wrapT: TextureWrapMode;
    private _minFilter: TextureMinFilter;
    private _magFilter: TextureMagFilter;
    protected _needsUpdate: boolean = false;

    // 静态默认占位贴图（2x2 RGBA 棋盘格）
    private static _defaultImageData: ImageData = (() => {
        const data = new Uint8ClampedArray([
            255, 255, 255, 255,   192, 192, 192, 255,
            192, 192, 192, 255,   255, 255, 255, 255
        ]);
        return new ImageData(data, 2, 2);
    })();

    constructor(options: TextureOptions) {
        this.name = options.name;
        this._wrapS = options.wrapS ?? TextureWrapMode.Repeat;
        this._wrapT = options.wrapT ?? TextureWrapMode.Repeat;
        this._minFilter = options.minFilter ?? TextureMinFilter.Linear;
        this._magFilter = options.magFilter ?? TextureMagFilter.Linear;

        if (options.source) {
            this.setSource(options.source);
        }
    }

    get image(): typeof this._image {
        return this._image;
    }

    set image(value: typeof this._image) {
        this._image = value;
        this._needsUpdate = true;
    }

    get wrapS(): TextureWrapMode {
        return this._wrapS;
    }

    set wrapS(value: TextureWrapMode) {
        this._wrapS = value;
        this._needsUpdate = true;
    }

    get wrapT(): TextureWrapMode {
        return this._wrapT;
    }

    set wrapT(value: TextureWrapMode) {
        this._wrapT = value;
        this._needsUpdate = true;
    }

    get minFilter(): TextureMinFilter {
        return this._minFilter;
    }

    set minFilter(value: TextureMinFilter) {
        this._minFilter = value;
        this._needsUpdate = true;
    }

    get magFilter(): TextureMagFilter {
        return this._magFilter;
    }

    set magFilter(value: TextureMagFilter) {
        this._magFilter = value;
        this._needsUpdate = true;
    }

    get gpuTexture(): WebGLTexture | GPUTexture | null {
        return this._gpuTexture;
    }

    // set gpuTexture(value: WebGLTexture | GPUTexture | null) {
    //     this._gpuTexture = value;
    // }

    // 静态 getter/setter
    public static get DefaultImageData(): ImageData {
        return this._defaultImageData;
    }
    public static set DefaultImageData(imgData: ImageData) {
        this._defaultImageData = imgData;
    }

    private setSource(source: TextureOptions["source"]) {
        if (typeof source === "string") {
            const img = new window.Image();
            img.crossOrigin = "anonymous";
            img.onload = () => {
                this.image = img;
            };
            img.onerror = () => {
                console.error(`Failed to load texture image: ${source}`);
            };
            img.src = source;

            // 在请求（异步）完成前，先设置为默认占位贴图
            this.image = Texture.DefaultImageData;
        } else {
            this._image = source ?? null;
        }

        this._needsUpdate = true;
    }

    private _loadImageElement(url: string): Promise<void> {
        return new Promise((resolve, reject) => {
            const img = new Image();
            if (isCrossOrigin(url)) {
                img.crossOrigin = "anonymous";
            }
            img.onload = () => {
                this.image = img;
                resolve();
            };
            img.onerror = (e) => { 
                console.error("Image load failed", url, e); 
                this.image = Texture.DefaultImageData;
                reject(e); 
            };
            img.src = url;
        });
    }
    
    private async tryFetchCreateImageBitmap(url: string): Promise<ImageBitmap | null> {
        try {
            const resp = await fetch(url, isCrossOrigin(url) ? { mode: 'cors' } : undefined);
            if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
            const blob = await resp.blob();
            if ("createImageBitmap" in window) {
                return await createImageBitmap(blob);
            }
            return null;
        } catch (e) {
            return null;
        }
    }

    private async setSourceFromUrl(url: string): Promise<void> {
        // 你可以根据实际需求选择优先级
        // 这里优先 fetch + createImageBitmap，失败后 fallback 到 <img>
        try {
            const bitmap = await this.tryFetchCreateImageBitmap(url);
            if (bitmap) {
                this.image = bitmap;
                return;
            }
        } catch (e) {
            // fetch/createImageBitmap 失败，fallback
        }
        // fallback 到 <img>
        await this._loadImageElement(url);
    }

    /**
     * 加载并设置纹理源，返回一个 Promise，
     * 在 Promise resolve 之前 this.image 始终指向占位图。
     * 
     * 不能直接在构造函数（constructor）里调用异步函数（async/await）
     * 
     * TODO: 未来由资源管理器统一异步加载和分发贴图，渲染时监听贴图 ready 状态
     */
    // public async setSource(source: TextureOptions["source"]): Promise<void> {
    //     this.image = Texture.DefaultImageData;
    //     if (!source) return;

    //     if (typeof source === "string") {
    //         await this.setSourceFromUrl(source);
    //     } else if (source instanceof ImageBitmap || source instanceof HTMLImageElement) {
    //         this.image = source;
    //     } else {
    //         this.image = Texture.DefaultImageData;
    //     }
    // }

    needsUpdate(): boolean {
        return this._needsUpdate;
    }

    markUpdated(): void {
        this._needsUpdate = false;
    }

    upload(context: Context, force: boolean = false): void {
        // if (this._gpuTexture && !force /* && !this._needsUpdate*/) return;

        // 1. 判断 imageData 尺寸
        const image = this.image ?? Texture.DefaultImageData;
        const width = image.width;
        const height = image.height;

        // 2. 判断是否需要重新创建GPU纹理
        // WebGL中：一旦创建了GPU纹理，其尺寸就算改变了，也不需要重新创建（因此调用该 upload 方法时，无须传 force 参数，保持默认值false即可）
        // WebGPU中：无论是否创建了GPU纹理，只要尺寸改变，都需要重新创建GPU纹理（因此调用该 upload 方法时，必须传 force 参数，其必须为 true）
        if (!this._gpuTexture || (force && (this._gpuTextureWidth !== width || this._gpuTextureHeight !== height))) {
            // 该逻辑专用于WebGPU
            if (this._gpuTexture) {
                context.deleteTexture(this._gpuTexture);
            }

            // 创建GPU纹理
            this._gpuTexture = context.createTexture({
                width: width,
                height: height,
                format: 'rgba8unorm',
                usage: GPUTextureUsage.TEXTURE_BINDING | GPUTextureUsage.COPY_DST | GPUTextureUsage.RENDER_ATTACHMENT,
            });

            this._gpuTextureWidth = width;
            this._gpuTextureHeight = height;
        }

        // 3. 上传数据
        // 无论WebGL还是WebGPU，只要纹理图片发生改变，都需要调用 writeTexture 方法重新上传数据
        if (this._needsUpdate) {
            context.writeTexture(this);
            this._needsUpdate = false;   // 只有在纹理图片数据上传后，才需要将needsUpdate设置为false
        }
    }

    /**
     * 动态更新已有 GPU 纹理的内容（不重建 GPU 纹理对象，仅写入新像素数据）
     * @param context 渲染上下文
     * @param options 可选参数，支持 partial update（如区域更新、不同数据源等）
     */
    updateTexture(
        context: Context,
        options?: {
            imageData?: ImageData | HTMLImageElement | HTMLCanvasElement | ImageBitmap | Uint8Array,
            x?: number,
            y?: number,
            width?: number,
            height?: number,
        }
    ): void {
        if (!this._gpuTexture) {
            console.warn('GPU纹理尚未创建，无法 updateTexture。请先调用 upload。');
            return;
        }

        // 默认全量更新
        if (!options || !options.imageData) {
            context.writeTexture(this);
            this._needsUpdate = false;
            return;
        }

        // 区域/自定义数据更新（需 context.writeTexture 支持区域写入，可根据后端扩展）
        // 这里假设 context.writeTexture 支持 options 透传
        // TODO: 未来再支持这种区域更新的接口
        context.writeTexture(this/*, options*/);
        this._needsUpdate = false;
    }

}
