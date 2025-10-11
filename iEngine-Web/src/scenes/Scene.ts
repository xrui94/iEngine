import { Camera } from '../views/cameras/Camera';
// import { WebGLRenderer } from '../renderers/webgl/WebGLRenderer';
// import { WebGPURenderer } from '../renderers/webgpu/WebGPURenderer';
import { WebGLContext, WebGLContextOptions } from '../renderers/webgl/WebGLContext';
import { WebGPUContext, WebGPUContextOptions } from '../renderers/webgpu/WebGPUContext';

import type { Light } from '../lights/Light';
import type { RendererType } from '../renderers/Renderer';
import type { Renderable } from '../core/Model';

export class Scene {

    ready: boolean = false;

    private _canvas: HTMLCanvasElement | null = null;

    // 使用私有变量存储当前活动相机
    private _activeCamera: Camera | null = null;

    // private _renderer: WebGLRenderer  | WebGPURenderer | null = null;

    private _activeContext!: WebGLContext | WebGPUContext;
    private _webglContext: WebGLContext;
    private _webgpuContext: WebGPUContext;

    // 存储场景中的所有组件
    private components: any[] = [];

    // 新增：光照数组
    private lights: Light[] = [];

    constructor(canvas: string | HTMLCanvasElement, options: WebGLContextOptions | WebGPUContextOptions) {
        // 获取 canvas 元素
        if (canvas instanceof HTMLCanvasElement) {
            this._canvas = canvas;
        } else if (typeof canvas === 'string') {
            // 如果传入的是字符串，则尝试获取对应的 canvas 元素
            this._canvas = document.getElementById(canvas) as HTMLCanvasElement;
        }
        // 确保 canvas 元素存在
        if (!this._canvas) {
            throw new Error('Canvas element not found');
        }

        this._webglContext = new WebGLContext(this._canvas, options);
        this._webgpuContext = new WebGPUContext(this._canvas, options);
    }

    get activeCamera(): Camera | null {
        return this._activeCamera;
    }

    // 设置当前活动相机
    // 注意：如果需要更复杂的相机管理，可以考虑使用相机管理器
    set activeCamera(cam: Camera | null) {
        this._activeCamera = cam;
    }

    // get renderer(): WebGLRenderer | WebGPURenderer | null {
    //     // 返回当前场景的渲染器
    //     // 注意：这里假设场景只使用一个渲染器
    //     return this._renderer;
    // }

    // set renderer(renderer: WebGLRenderer | WebGPURenderer | null) {
    //     // 设置当前场景的渲染器
    //     this._renderer = renderer;
    // }   

    get context(): WebGLContext | WebGPUContext {
        // 返回当前场景的渲染器
        // 注意：这里假设场景只使用一个渲染器
        return this._activeContext;
    }

    set context(renderer: RendererType) {
        // 设置当前场景的渲染器
        if (renderer === 'webgl') {
            this._activeContext = this._webglContext;
        } else if (renderer === 'webgpu') {
            this._activeContext = this._webgpuContext;
        } else {
            throw new Error(`Invalid renderer type: ${renderer}, it must be one of "webgl" or "webgpu"`);
        }
    }   

    addComponent(component: any): void {
        this.components.push(component);
    }

    removeComponent(component: any): void {
        const index = this.components.indexOf(component);
        if (index !== -1) {
            this.components.splice(index, 1);
        }
    }

    getComponents(): Renderable[] {
        return this.components.slice();
    }

    addLight(light: Light): void {
        this.lights.push(light);
    }

    removeLight(light: Light): void {
        const idx = this.lights.indexOf(light);
        if (idx !== -1) this.lights.splice(idx, 1);
    }

    getLights(): Light[] {
        return this.lights.slice();
    }

    update(deltaTime: number): void {
        for (const comp of this.components) {
            if (typeof comp.update === 'function') {
                comp.update(deltaTime);
            }
        }
    }

    // render(context: WebGLRenderingContext): void {
    //     // 检查渲染器是否已经存在
    //     if (!this._renderer) {
    //         throw new Error('Renderer is not set for the scene');
    //     }

    //     //
    //     for (const comp of this.components) {
    //         if (typeof comp.render === 'function') {
    //             comp.render(context);
    //         }
    //     }
    // }
}