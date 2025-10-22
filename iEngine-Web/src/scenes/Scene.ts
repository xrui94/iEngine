import { Camera } from '../views/cameras/Camera';
import { WebGLContext, WebGLContextOptions } from '../renderers/webgl/WebGLContext';
import { WebGPUContext, WebGPUContextOptions } from '../renderers/webgpu/WebGPUContext';

import { Node } from '../core/Node';
import { RenderableComponent } from '../renderers/RenderableComponent';
import { LightComponent } from '../lights/LightComponent';
import { Matrix4 } from '../math/Matrix4';

import type { Light } from '../lights/Light';
import type { RendererType } from '../renderers/Renderer';
import type { Renderable } from '../renderers/Renderable';

export class Scene {

    ready: boolean = false;

    private _canvas: HTMLCanvasElement | null = null;

    // 使用数组存储多个相机
    private _cameras: Camera[] = [];
    // 当前活动相机
    private _activeCamera: Camera | null = null;

    private _activeContext!: WebGLContext | WebGPUContext;
    private _webglContext: WebGLContext;
    private _webgpuContext: WebGPUContext;

    private _root: Node;

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

        this._root = new Node('SceneRoot');
    }

    // 获取所有相机
    get cameras(): Camera[] {
        return this._cameras.slice();
    }

    // 添加相机到场景
    addCamera(camera: Camera): void {
        this._cameras.push(camera);
        // 如果还没有活动相机，设置第一个添加的相机为活动相机
        if (!this._activeCamera) {
            this._activeCamera = camera;
        }
    }

    // 从场景中移除相机
    removeCamera(camera: Camera): void {
        const index = this._cameras.indexOf(camera);
        if (index !== -1) {
            this._cameras.splice(index, 1);
            // 如果移除的是活动相机，设置下一个相机为活动相机（如果有的话）
            if (this._activeCamera === camera) {
                this._activeCamera = this._cameras.length > 0 ? this._cameras[0] : null;
            }
        }
    }

    // 获取当前活动相机
    get activeCamera(): Camera | null {
        return this._activeCamera;
    }

    // 设置当前活动相机
    set activeCamera(cam: Camera | null) {
        if (cam && this._cameras.indexOf(cam) === -1) {
            // 如果设置的相机不在场景中，先添加到场景中
            this._cameras.push(cam);
        }
        this._activeCamera = cam;
    }

    get context(): WebGLContext | WebGPUContext {
        return this._activeContext;
    }

    set context(renderer: RendererType) {
        if (renderer === 'webgl') {
            this._activeContext = this._webglContext;
        } else if (renderer === 'webgpu') {
            this._activeContext = this._webgpuContext;
        } else {
            throw new Error(`Invalid renderer type: ${renderer}, it must be one of "webgl" or "webgpu"`);
        }
    }   

    // 添加实体到场景
    addEntity(entity: Node): void {
        this._root.addChild(entity);
    }

    // 从场景中移除实体
    removeEntity(entity: Node): void {
        this._root.removeChild(entity);
    }

    // 获取场景中的所有实体
    getEntities(): Node[] {
        return this._root.getChildren().slice();
    }

    update(deltaTime: number): void {
        // 更新ECS实体（通过根节点遍历所有子节点）
        this._root.update(deltaTime);
    }

    // 收集所有 Renderable（供 Renderer 使用）
    collectRenderables(out: Renderable[]): void {
        // 收集ECS实体中的可渲染对象（通过根节点遍历所有子节点）
        this.traverse(this._root, node => {
            const comp = node.getComponent(RenderableComponent);
            if (comp) {
                out.push({
                    mesh: comp.mesh,
                    material: comp.material,
                    worldTransform: node.getWorldTransform(),
                    layer: comp.layer,
                });
            }
        });
    }
    
    // 收集所有光源（供 Renderer 使用）
    collectLights(out: Light[]): void {
        // 收集ECS实体中的光源组件（通过根节点遍历所有子节点）
        this.traverse(this._root, node => {
            const lightComp = node.getComponent(LightComponent);
            if (lightComp) {
                out.push(lightComp.light);
            }
        });
    }

    // 递归遍历场景树
    private traverse(node: Node, callback: (node: Node) => void): void {
        callback(node);
        for (const child of node.getChildren()) {
            this.traverse(child, callback);
        }
    }
    
}