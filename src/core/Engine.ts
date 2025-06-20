import { WebGLRenderer } from '../renderers/webgl/WebGLRenderer';
import { WebGPURenderer } from '../renderers/webgpu/WebGPURenderer';
//
import { registerBuiltInShaders } from '../shaders/ShaderLib';
import { registerBuiltInMaterials } from '../materials/MaterialManager';
//
import { Scene } from '../scenes/Scene';

import { Renderer, RendererType } from '../renderers/Renderer';
import type { WebGLContextOptions, WebGLContext } from '../renderers/webgl/WebGLContext';
import type { WebGPUContextOptions, WebGPUContext } from '../renderers/webgpu/WebGPUContext';

export type EngineOptions = {
    renderer?: RendererType; // 默认使用 WebGL 渲染器
    // useWebGPU?: boolean; // 是否使用 WebGPU 渲染器
    disableWebGPU?: boolean; // 是否禁用 WebGPU 支持
    // webGLOptions?: WebGLContextOptions;
    // webGPUOptions?: WebGPUContextOptions
};

export class Engine {
    // private canvas: HTMLCanvasElement | null = null;
    private activeRenderer: WebGLRenderer | WebGPURenderer | null = null;
    private webglRenderer: WebGLRenderer;
    private webgpuRenderer: WebGPURenderer | null = null;
    private scenes: Map<string, Scene> = new Map();
    private activeScene: Scene | null = null;
    private animationFrameId: number = 0;
    private lastTime: number = performance.now();
    
    constructor(/*canvas: string | HTMLCanvasElement, */options: EngineOptions = {}) {
        // 使用解构并设置默认值
        // 默认使用WebGL2作为图形API进行渲染
        const {
            renderer = options.renderer ?? 'webgl',
            // webGLOptions = options.webGLOptions || {},
            // webGPUOptions = options.webGPUOptions || {},
            disableWebGPU = false
        } = options;
        if (!Renderer.isRendererType(renderer)) {
            throw new Error(`Invalid renderer type: ${renderer}, it must be one of "webgl" or "webgpu"`);
        }

        // 获取 canvas 元素
        // if (canvas instanceof HTMLCanvasElement) {
        //     this.canvas = canvas;
        // } else if (typeof canvas === 'string') {
        //     // 如果传入的是字符串，则尝试获取对应的 canvas 元素
        //     this.canvas = document.getElementById(canvas) as HTMLCanvasElement;
        // }
        // 确保 canvas 元素存在
        // if (!this.canvas) {
        //     throw new Error('Canvas element not found');
        // }
        
        this.webglRenderer = new WebGLRenderer(/*this.canvas, webGLOptions*/);
        // 如果禁用 WebGPU，则将其设置为 null
        this.webgpuRenderer = disableWebGPU ? null : new WebGPURenderer(/*this.canvas, webGPUOptions*/);

        // 
        if (renderer === 'webgl') {
            this.activeRenderer = this.webglRenderer;
        } else if (renderer === 'webgpu') {
            if (disableWebGPU) {
                throw new Error('WebGPU is disabled, cannot use WebGPU renderer');
            }
            this.activeRenderer = this.webgpuRenderer;
        } else {
            throw new Error(`Unsupported renderer type: ${renderer}`);
        }

        if (!this.activeRenderer) {
            throw new Error('No active renderer set');
        }
        
        // 设置窗口调整事件
        window.addEventListener('resize', () => this.activeRenderer?.resize());
    }

    private async _initRenderer(): Promise<void> {
        if (!this.activeScene) {
            throw new Error('No active scene set');
        }

        if (!this.activeRenderer) {
            throw new Error('No active renderer set');
        }

        if (this.activeRenderer instanceof WebGLRenderer) {
            this.activeRenderer.init(this.activeScene.context as WebGLContext);
        } else {
            await this.activeRenderer.init(this.activeScene.context as WebGPUContext);
        }
    }
    
    private _update(deltaTime: number): void {
        if (this.activeScene) {
            this.activeScene.update(deltaTime);
        }
    }

    private _render(): void {
        if (this.activeScene && this.activeRenderer) {
            this.activeRenderer.render(this.activeScene);
        }
    }

     // 主循环函数
    // 使用 requestAnimationFrame 来实现高效的渲染循环
    private _tick = () => {
        const now = performance.now();
        const deltaTime = (now - this.lastTime) / 1000;
        this.lastTime = now;

        // 1. 更新动画、和其它逻辑
        this._update(deltaTime);

        // 2. 渲染
        this._render();

        this.animationFrameId = requestAnimationFrame(this._tick);
    }
    
    async start() {
        await this._initRenderer();

        // 注册内置 Shader
        registerBuiltInShaders();

        // 注册内置材质
        registerBuiltInMaterials();

        // 初始化所有材质的 Shader（可选缓存）
        // MaterialManager.warmUpShaders(this.renderer.backend);

        this.lastTime = performance.now();
        this._tick();
    }
    
    stop(): void {
        cancelAnimationFrame(this.animationFrameId);
    }

    getScene(name: string): Scene | undefined {
        return this.scenes.get(name);
    }

    addScene(name: string, scene: Scene): void {
        this.scenes.set(name, scene);
        if (!this.activeScene) {
            this.setActiveScene(name);
        }
    }

    setActiveScene(name: string): void {
        const scene = this.scenes.get(name);
        if (scene) {
            // scene.renderer = this.activeRenderer; // 设置渲染器
            scene.context = this.activeRenderer instanceof WebGLRenderer ? 'webgl' : 'webgpu';
            this.activeScene = scene;
        } else {
            throw new Error(`Scene "${name}" not found`);
        }
    }
    
    async setRenderer(renderer: RendererType): Promise<void> {
        if (!this.activeScene) {
            throw new Error('No active scene set');
        }

        if (!this.activeRenderer) {
            throw new Error('No active renderer set');
        }

        if (renderer === 'webgl') {
            this.activeRenderer = this.webglRenderer;
            this.activeScene.context = 'webgl';
        } else if (renderer === 'webgpu') {
            this.activeRenderer = this.webgpuRenderer;
            this.activeScene.context = 'webgpu';
        }

        await this._initRenderer();

        // if (this.activeScene) {
        //     this.activeScene.renderer = this.activeRenderer;
        //     // 关键：重置所有 mesh 的 uploaded 标记
        //     for (const comp of this.activeScene.getComponents()) {
        //         if (comp.mesh) {
        //             comp.mesh.uploaded = false;
        //         }
        //     }
        // }
    }
}
