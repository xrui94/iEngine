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
    private resourcesRegistered: boolean = false; // 资源注册状态标记
    private isReady: boolean = false; // 引擎是否已准备就绪
    private animationFrameId: number = 0;
    private lastTime: number = performance.now();
    private animationLoop: (() => void) | null = null;
    private isRunning: boolean = false;
    
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

    private async _initRenderer(): Promise<boolean> {
        if (!this.activeRenderer) {
            throw new Error('No active renderer set');
        }

        if (!this.activeScene) {
            return false;
        }

        if (this.activeRenderer instanceof WebGLRenderer) {
            this.activeRenderer.init(this.activeScene.context as WebGLContext);
        } else {
            await this.activeRenderer.init(this.activeScene.context as WebGPUContext);
        }
        
        return true;
    }

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

     // 主循环函数
    // 使用 requestAnimationFrame 来实现高效的渲染循环
    tick(): void {
        // 检查引擎是否已准备就绪
        if (!this.isReady) {
            throw new Error('Engine is not ready. Please call start() before tick().');
        }

        // 如果没有活动场景或渲染器未初始化，静默返回
        if (!this.activeScene || !this.activeRenderer?.isInitialized()) {
            // throw new Error('No active scene or renderer is not initialized');
            // 如果没有活动场景或渲染器未初始化，静默返回而不是抛出异常!!!
            return;
        }

        //
        const now = performance.now();
        const deltaTime = (now - this.lastTime) / 1000;
        this.lastTime = now;

        // 1. 更新动画、和其它逻辑
        this._update(deltaTime);

        // 2. 渲染
        this._render();
    }
    
    /**
     * 初始化引擎
     * 注意：此方法不启动渲染循环，需调用 setAnimationLoop 来控制渲染
     * 渲染器初始化将在设置活动场景时自动完成
     */
    start() {
        // const rendererInitialized = await this._initRenderer();

        // // 只有在渲染器初始化成功时才注册资源
        // if (rendererInitialized) {
        //     // 注册内置 Shader
        //     registerBuiltInShaders();

        //     // 注册内置材质
        //     registerBuiltInMaterials();

        //     // 初始化所有材质的 Shader（可选缓存）
        //     // MaterialManager.warmUpShaders(this.renderer.backend);
        // }

        // 注册内置资源（如果尚未注册）
        // this._registerResources();

        // 注册内置资源（如果尚未注册）
        this._registerResources();

        // 标记引擎已准备就绪
        this.isReady = true;
        
        // 如果已有活动场景，则初始化渲染器
        // if (this.activeScene) {
        //     // 异步初始化在后台进行，不阻塞用户代码
        //     this._initRenderer().then(success => {
        //         if (!success) {
        //             console.warn('Failed to initialize renderer');
        //         }
        //     }).catch(error => {
        //         console.warn('Error initializing renderer:', error);
        //     });
        // }
        // 如果没有活动场景，渲染器将在 addScene 或 setActiveScene 时，
        // 尝试再次通过 start 方法来启动引擎，以初始化
    }
    
    stop(): void {
        this.setAnimationLoop(null); // 复用现有逻辑
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

            // 如果渲染器尚未初始化（因为之前没有场景），则在此处初始化
            if (this.activeRenderer && !this.activeRenderer.isInitialized()) {
                // 异步初始化在后台进行，不阻塞用户代码
                this._initRenderer().then(success => {
                    // if (success) {
                    //     this._registerResources();
                    // }
                    if (!success) {
                        console.error('Failed to initialize renderer');
                    }
                }).catch(error => {
                    console.error('Error initializing renderer:', error);
                });
            }
        } else {
            throw new Error(`Scene "${name}" not found`);
        }
    }
    
    /*async */setRenderer(renderer: RendererType)/*: Promise<void>*/ {
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

        // await this._initRenderer();
        // 异步初始化在后台进行，不阻塞用户代码
        this._initRenderer().then(success => {
            // if (success) {
            //     this._registerResources();
            // }
            if (!success) {
                console.error('Failed to initialize renderer');
            }
        }).catch(error => {
            console.error('Error initializing renderer:', error);
        });

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
