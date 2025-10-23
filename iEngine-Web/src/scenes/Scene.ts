import { Camera } from '../views/cameras/Camera';

import { Node } from '../core/Node';
import { RenderableComponent } from '../renderers/RenderableComponent';
import { LightComponent } from '../lights/LightComponent';
import { Matrix4 } from '../math/Matrix4';
import { Transform } from '../ecs/components/TransformECS';
import { LightECS } from '../ecs/components/LightECS';
import { newEntityId } from '../ecs/Entity';

import type { Light } from '../lights/Light';
import type { RendererType } from '../renderers/Renderer';
import type { Renderable } from '../renderers/Renderable';
import { WorldECS } from '../ecs/WorldECS';
import { World } from './World';
import { SystemManagerECS } from '../ecs/SystemManager';
import { TransformSystem } from '../ecs/systems/TransformSystem';
import { LightSystem } from '../ecs/systems/LightSystem';
import { RenderSystem } from '../ecs/systems/RenderSystem';

export class Scene {

    ready: boolean = false;



    // 使用数组存储多个相机
    private _cameras: Camera[] = [];
    // 当前活动相机
    private _activeCamera: Camera | null = null;


    private _root: Node;
    private _ecsWorld: WorldECS;
    private _world: World;
    private _ecsSystemManager: SystemManagerECS;
    private _renderSystem: RenderSystem;

    constructor() {
        // OOP组件架构
        this._root = new Node('SceneRoot');
        this._root.setOwnerScene(this);
        this._world = new World();

        // ECS设计架构
        this._ecsWorld = new WorldECS();
        this._ecsSystemManager = new SystemManagerECS();
        this._ecsSystemManager.addSystem(new TransformSystem());
        this._ecsSystemManager.addSystem(new LightSystem());
        const renderSys = new RenderSystem();
        this._ecsSystemManager.addSystem(renderSys);
        this._renderSystem = renderSys;
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

    // 暴露 ECS 世界给外部使用
    getECSWorld(): WorldECS {
        return this._ecsWorld;
    }

    // 暴露 OOP World 给外部使用
    getWorld(): World {
        return this._world;
    }

    // 便捷：暴露 ECS 系统管理器（如需外部访问）
    getECSSystemManager(): SystemManagerECS {
        return this._ecsSystemManager;
    }

    // 添加实体到场景
    addEntity(entity: Node): void {
        entity.setOwnerScene(this);
        this._root.addChild(entity);
    }

    // 便捷添加光源：OOP 组件 + 可选 ECS 注册
    addLight(light: Light, useECS: boolean = false): void {
        const node = new Node(light?.constructor?.name ?? 'Light');
        const lightComponent = new LightComponent(light);
        node.addComponent(lightComponent);
        this.addEntity(node);

        if (useECS) {
            const eid = newEntityId();
            this._ecsWorld.createEntity(eid);
            this._ecsWorld.addComponent(eid, 'LightECS', new LightECS(light));
            this._ecsWorld.addComponent(eid, 'Transform', new Transform());
        }
    }

    // 从场景中移除实体
    removeEntity(entity: Node): void {
        entity.setOwnerScene(undefined);
        this._root.removeChild(entity);
    }

    // 获取场景中的所有实体
    getEntities(): Node[] {
        return this._root.getChildren().slice();
    }

    update(deltaTime: number): void {
        // 更新OOP实体树
        this._root.update(deltaTime);
        // 更新所有ECS系统（Transform、Light等）
        this._ecsSystemManager.update(this._ecsWorld, deltaTime);
        // 在每帧更新后，构建轻量 World 视图供渲染器消费
        this._world.scanScene(this);
    }

    // 统一查询接口：返回当前场景的渲染对象与光源
    getRenderables(): Renderable[] {
        const arr: Renderable[] = [];
        // 来自轻量 World 的 OOP 渲染对象
        arr.push(...this._world.getRenderables());
        // 来自 ECS 的渲染对象（RenderSystem 缓存）
        arr.push(...this._renderSystem.getRenderables());
        return arr;
    }

    getLights(): Light[] {
        const arr: Light[] = [];
        // 来自轻量 World 的光源
        arr.push(...this._world.getLights());
        // 来自 ECSWorld 的光源
        for (const le of this._ecsWorld.getLights()) arr.push(le.light);
        return arr;
    }

    // 递归遍历场景树
    private traverse(node: Node, callback: (node: Node) => void): void {
        callback(node);
        for (const child of node.getChildren()) {
            this.traverse(child, callback);
        }
    }
    
}