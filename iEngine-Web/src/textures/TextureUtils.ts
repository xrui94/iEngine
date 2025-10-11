import { TextureWrapMode, TextureMinFilter, TextureMagFilter } from "./Texture";

// WebGL 映射
export function getWebGLWrapMode(gl: WebGLRenderingContext, mode: TextureWrapMode): number {
    switch (mode) {
        case TextureWrapMode.Repeat: return gl.REPEAT;
        case TextureWrapMode.ClampToEdge: return gl.CLAMP_TO_EDGE;
        case TextureWrapMode.MirroredRepeat: return gl.MIRRORED_REPEAT;
        default: return gl.REPEAT;
    }
}

export function getWebGLMinFilter(gl: WebGLRenderingContext, filter: TextureMinFilter): number {
    switch (filter) {
        case TextureMinFilter.Nearest: return gl.NEAREST;
        case TextureMinFilter.Linear: return gl.LINEAR;
        case TextureMinFilter.NearestMipmapNearest: return gl.NEAREST_MIPMAP_NEAREST;
        case TextureMinFilter.LinearMipmapNearest: return gl.LINEAR_MIPMAP_NEAREST;
        case TextureMinFilter.NearestMipmapLinear: return gl.NEAREST_MIPMAP_LINEAR;
        case TextureMinFilter.LinearMipmapLinear: return gl.LINEAR_MIPMAP_LINEAR;
        default: return gl.LINEAR;
    }
}

export function getWebGLMagFilter(gl: WebGLRenderingContext, filter: TextureMagFilter): number {
    switch (filter) {
        case TextureMagFilter.Nearest: return gl.NEAREST;
        case TextureMagFilter.Linear: return gl.LINEAR;
        default: return gl.LINEAR;
    }
}

// WebGPU 映射
export function getWebGPUWrapMode(mode: TextureWrapMode): GPUAddressMode {
    switch (mode) {
        case TextureWrapMode.Repeat: return 'repeat';
        case TextureWrapMode.ClampToEdge: return 'clamp-to-edge';
        case TextureWrapMode.MirroredRepeat: return 'mirror-repeat';
        default: return 'repeat';
    }
}

export function getWebGPUMinMagFilter(filter: TextureMinFilter | TextureMagFilter): GPUFilterMode {
    // WebGPU 只支持 'nearest' | 'linear'
    switch (filter) {
        case TextureMinFilter.Nearest:
        case TextureMagFilter.Nearest:
            return 'nearest';
        case TextureMinFilter.Linear:
        case TextureMagFilter.Linear:
        default:
            return 'linear';
    }
}