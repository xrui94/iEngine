import { Scene } from '../scenes/Scene';

export type RendererType = 'webgl' | 'webgpu';
export type GraphicsAPI = 'webgl1' | 'webgl2' | 'webgpu';

export abstract class Renderer {
    // abstract getOrCreateShader(
    //     shaderName: string,
    //     defines: Record<string, string | number | boolean>
    // ): WebGLShaderProgram | WebGPUShaderModule | null;

    static isRendererType(value: any): value is RendererType {
        return value === 'webgl' || value === 'webgpu';
    }

    static isGraphicsAPI(value: any): value is GraphicsAPI {
        return value === 'webgl1' || value === 'webgl2' || value === 'webgpu';
    }

    abstract render(scene: Scene): void;

    // abstract setCamera(camera: Camera): void;
    abstract resize(): void;

    abstract clear(): void;
}
