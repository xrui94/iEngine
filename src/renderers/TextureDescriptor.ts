export interface TextureDescriptor {
    width: number;
    height: number;
    format?: GPUTextureFormat;             // WebGL: gl.RGBA, WebGPU: 'rgba8unorm'等
    usage?: GPUTextureUsageFlags;              // WebGPU: usage flags
    mipLevelCount?: number;
    label?: string;
    // ...可扩展
}