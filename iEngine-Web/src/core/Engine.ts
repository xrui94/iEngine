import { WebGLRenderer } from '../renderers/webgl/WebGLRenderer';
import { WebGPURenderer } from '../renderers/webgpu/WebGPURenderer';
//
import { registerBuiltInShaders } from '../shaders/ShaderLib';
import { registerBuiltInMaterials } from '../materials/MaterialManager';
//
import { Scene } from '../scenes/Scene';
import { Renderer, RendererType } from '../renderers/Renderer';

import type { WebGLContextOptions } from '../renderers/webgl/WebGLContext';
import type { WebGPUContextOptions } from '../renderers/webgpu/WebGPUContext';



export type EngineOptions = {
    renderer?: RendererType; // 默认使用 WebGL 渲染器
    // useWebGPU?: boolean; // 是否使用 WebGPU 渲染器
    disableWebGPU?: boolean; // 是否禁用 WebGPU 支持
    webGLOptions?: WebGLContextOptions;
    webGPUOptions?: WebGPUContextOptions
};

export class Engine {
    // private _canvas: HTMLCanvasElement = null!;
    // private _webglOptions: WebGLContextOptions = {};
    // private _webgpuOptions: WebGPUContextOptions = {};

    //
    // private webglRenderers: WebGLRenderer[] = [];
    // private webgpuRenderer: WebGPURenderer | null = null;

    //
    private resourcesRegistered: boolean = false; // 资源注册状态标记
    private isReady: boolean = false; // 引擎是否已准备就绪
    private animationFrameId: number = 0;
    private lastTime: number = performance.now();
    private animationLoop: (() => void) | null = null;  // 动画循环回调函数
    private isRunning: boolean = false;
    
    constructor(/*canvas: string | HTMLCanvasElement, options: EngineOptions = {}*/) {
        // // 使用解构并设置默认值
        // // 默认使用WebGL2作为图形API进行渲染
        // const {
        //     renderer = options.renderer ?? 'webgl',
        //     webGLOptions = options.webGLOptions || {},
        //     webGPUOptions = options.webGPUOptions || {},
        //     disableWebGPU = false
        // } = options;
        // this._webglOptions = webGLOptions;
        // this._webgpuOptions = webGPUOptions;

        // // 获取 canvas 元素 
        // if (!(canvas instanceof HTMLCanvasElement) && typeof canvas === 'string') {
        //     const htmlElement = document.getElementById(canvas);
        //     if (htmlElement instanceof HTMLCanvasElement) {
        //         this._canvas = htmlElement;
        //     } else {
        //         throw new Error('Canvas element not found');
        //     }
        // } else {
        //     this._canvas = canvas;
        // }

        // if (!Renderer.isRendererType(renderer)) {
        //     throw new Error(`Invalid renderer type: ${renderer}, it must be one of "webgl" or "webgpu"`);
        // }

        // 初始化渲染器
        // this.webglRenderers.push(new WebGLRenderer());
        // this.webgpuRenderer = new WebGPURenderer();

        this._registerResources();

        // 设置窗口调整事件
        // window.addEventListener('resize', () => this.activeRenderer?.resize());
    }

    /**
     * 注册引擎内置的基础资源（一次性）
     * @returns 
     */
    private _registerResources(): void {
        if (this.resourcesRegistered) {
            return; // 避免重复注册
        }
        
        // 注册内置 Shader
        registerBuiltInShaders();

        // 注册内置材质
        registerBuiltInMaterials();
        
        this.resourcesRegistered = true;
    }

    // private async _initRenderer(): Promise<boolean> {
    //     if (!this.activeRenderer) {
    //         throw new Error('No active renderer set');
    //     }

    //     if (!this.activeScene) {
    //         return false;
    //     }

    //     if (this.activeRenderer instanceof WebGLRenderer) {
    //         this.activeRenderer.init(this._canvas, this._webglOptions);
    //     } else {
    //         await this.activeRenderer.init(this._canvas, this._webgpuOptions);
    //     }
        
    //     return true;
    // }

    private _runAnimationLoop = () => {
        if (this.animationLoop && this.isRunning) {
            this.animationLoop();
            this.animationFrameId = requestAnimationFrame(this._runAnimationLoop);
        }
    }
    
    private _stopAnimationLoop(): void {
        if (this.animationFrameId) {
            cancelAnimationFrame(this.animationFrameId);
            this.animationFrameId = 0;
        }
        this.isRunning = false;
    }

    // 核心API：设置自定义动画循环（Three.js风格）
    setAnimationLoop(callback: (() => void) | null): void {
        // 停止当前循环
        this._stopAnimationLoop();
        
        this.animationLoop = callback;
        
        if (callback) {
            this.isRunning = true;
            this._runAnimationLoop();
        } else {
            this.isRunning = false;
        }
    }

}