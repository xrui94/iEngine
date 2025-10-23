// render/RenderView.ts

import type { Renderer } from '../renderers/Renderer';
import type { Scene } from '../scenes/Scene';
import type { WebGLContextOptions } from '../renderers/webgl/WebGLContext';
import type { WebGPUContextOptions } from '../renderers/webgpu/WebGPUContext';

import { WebGLRenderer } from '../renderers/webgl/WebGLRenderer';
import { WebGPURenderer } from '../renderers/webgpu/WebGPURenderer';

export interface RenderViewOptions {
    canvas: HTMLCanvasElement;
    backend?: 'webgl' | 'webgpu';
    webgl?: WebGLContextOptions;
    webgpu?: WebGPUContextOptions;
    scene: Scene;
    renderer: Renderer;
    viewport?: { x: number; y: number; width: number; height: number };
    autoResize?: boolean;
    clearColor?: [number, number, number, number];
}

export class RenderView {
    public readonly canvas: HTMLCanvasElement;
    public readonly scene: Scene;
    public readonly renderer: Renderer;
    public readonly viewport?: { x: number; y: number; width: number; height: number };
    public readonly autoResize: boolean;
    public readonly clearColor?: [number, number, number, number];

    private _initialized: boolean = false;
    private _resizeHandler: (() => void) | null = null;

    constructor(options: RenderViewOptions) {
         // 获取 canvas 元素 
        if (!(options.canvas instanceof HTMLCanvasElement)) {
            throw new Error('Invalid canvas element');
        }

        this.canvas = options.canvas;

        // if (options.backend !== 'webgl' && options.backend !== 'webgpu') {
        //     throw new Error('Invalid backend specified');
        // }
        
        this.scene = options.scene;
        this.renderer = options.renderer;

        // 如果未提供 viewport，则不设置，让渲染器在 render 时默认使用全屏绘制缓冲尺寸
        this.viewport = options.viewport;

        this.autoResize = options.autoResize ?? true;
        this.clearColor = options.clearColor;
    }

    async init(): Promise<void> {
        if (this._initialized) return;

        // 区分初始化参数
        if (this.renderer instanceof WebGLRenderer) {
            await Promise.resolve(this.renderer.init(this.canvas, { useWebGL1: false }));
        } else if (this.renderer instanceof WebGPURenderer) {
            await this.renderer.init(this.canvas, {});
        } else {
            throw new Error('Unsupported renderer type');
        }

        // 设置 clearColor 和 viewport（如果提供）
        if (this.clearColor) {
            this.renderer.setClearColor(this.clearColor);
        }
        if (this.viewport) {
            this.renderer.setViewport(this.viewport);
        }

        this._initialized = true;

        // 初始化时，需要先执行一次resize
        this.renderer.resize();

        // 监听窗口 resize 事件（如果启用 autoResize）
        if (this.autoResize) {
            this._resizeHandler = () => this.renderer.resize();
            window.addEventListener('resize', this._resizeHandler);
        }
    }

    render(deltaTime?: number, options?: { clearCanvas?: boolean }): void {
        if (!this._initialized) return;

        // this.renderer.resize();

        // 每帧应用视口，避免resize或其他状态重置
        if (this.viewport) {
            this.renderer.setViewport(this.viewport);
        } else {
            this.renderer.setViewport(); // 使用全屏默认
        }

        // 每帧应用本视图的 clearColor，避免共享渲染器状态被其他视图覆盖
        if (this.clearColor) {
            this.renderer.setClearColor(this.clearColor);
        }

        this.scene.update(deltaTime || 0);
        this.renderer.render(this.scene, options);
    }

    isInitialized(): boolean {
        return this._initialized;
    }

    dispose(): void {
        if (this._resizeHandler) {
            window.removeEventListener('resize', this._resizeHandler);
            this._resizeHandler = null;
        }
        // 可选：调用 renderer.dispose()（如果实现）
    }
}