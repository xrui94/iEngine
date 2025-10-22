import { Scene } from '../scenes/Scene';

export type RendererType = 'webgl' | 'webgpu';
export type GraphicsAPI = 'webgl1' | 'webgl2' | 'webgpu';

export abstract class Renderer {
    static isRendererType(value: any): value is RendererType {
        return value === 'webgl' || value === 'webgpu';
    }

    static isGraphicsAPI(value: any): value is GraphicsAPI {
        return value === 'webgl1' || value === 'webgl2' || value === 'webgpu';
    }

    abstract isInitialized(): boolean;

    // 渲染方法，只接收场景参数
    abstract render(scene: Scene): void;

    abstract resize(): void;

    abstract clear(): void;
}