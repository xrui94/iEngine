// 未来方便未来 C++ 版本引入 ECS 架构，因此：
// Node 不包含任何渲染/更新逻辑，只提供 getWorldTransform()
import { Matrix4, Vector3 } from '../math';
import { Component } from './Component';
import type { Scene } from '../scenes/Scene';

export class Node {
    private localPosition: Vector3 = new Vector3();
    private localRotation: Vector3 = new Vector3(); // 可扩展为 quat
    private localScale: Vector3 = new Vector3(1, 1, 1);

    private worldTransform: Matrix4 = new Matrix4();
    private transformDirty = true;

    private parent: Node | null = null;
    private children: Node[] = [];
    private components: Component[] = [];
    
    public name: string;
    private ownerScene?: Scene;

    constructor(name: string = 'Node') {
        this.name = name;
    }

    // --- 变换 API ---
    setLocalPosition(x: number, y: number, z: number): void {
        this.localPosition.set(x, y, z);
        this.markTransformDirty();
    }
    
    getLocalPosition(): Vector3 {
        // 创建一个新的Vector3实例并复制当前值
        return new Vector3(this.localPosition.x, this.localPosition.y, this.localPosition.z);
    }

    getWorldTransform(): Matrix4 {
        if (this.transformDirty) this.updateWorldTransform();
        return this.worldTransform;
    }

    // --- 层级管理 ---
    addChild(child: Node): void {
        if (child.parent) {
            child.parent.removeChild(child);
        }
        child.parent = this;
        child.markTransformDirty();
        this.children.push(child);
        // 继承所属场景引用
        child.setOwnerScene(this.ownerScene);
    }

    removeChild(child: Node): void {
        const index = this.children.indexOf(child);
        if (index !== -1) {
            this.children.splice(index, 1);
            child.parent = null;
            // 移除所属场景引用
            child.setOwnerScene(undefined);
        }
    }

    getChildren(): readonly Node[] {
        return this.children;
    }

    getParent(): Node | null {
        return this.parent;
    }

    // --- 组件管理 ---
    addComponent<T extends Component>(component: T): T {
        this.components.push(component);
        component.setNode(this);
        component.onAttach();
        return component;
    }

    getComponent<T extends Component>(type: new (...args: any[]) => T): T | undefined {
        return this.components.find(c => c instanceof type) as T | undefined;
    }
    
    getComponentsByType<T extends Component>(type: new (...args: any[]) => T): T[] {
        return this.components.filter(c => c instanceof type) as T[];
    }

    removeComponent<T extends Component>(component: T): boolean {
        const index = this.components.indexOf(component);
        if (index !== -1) {
            component.onDetach();
            component.setNode(null);
            this.components.splice(index, 1);
            return true;
        }
        return false;
    }

    getComponents(): readonly Component[] {
        return [...this.components]; // 返回副本
    }
    
    // 更新所有组件
    update(deltaTime: number): void {
        for (const component of this.components) {
            component.onUpdate(deltaTime);
        }
        
        // 递归更新子节点
        for (const child of this.children) {
            child.update(deltaTime);
        }
    }

    // --- 所属场景管理 ---
    setOwnerScene(scene: Scene | undefined): void {
        this.ownerScene = scene;
        for (const child of this.children) {
            child.setOwnerScene(scene);
        }
    }

    // --- 内部 ---
    private markTransformDirty(): void {
        this.transformDirty = true;
        // 移除 HybridCoordinator 后，不再自动标记场景桥接增量同步
        for (const child of this.children) {
            child.markTransformDirty();
        }
    }

    private updateWorldTransform(): void {
        if (!this.transformDirty) return;

        this.worldTransform.identity();
        this.worldTransform.multiply(Matrix4.fromTranslation(this.localPosition));
        // 可扩展旋转、缩放

        if (this.parent) {
            const parentWorld = this.parent.getWorldTransform();
            const tempMatrix = new Matrix4();
            tempMatrix.multiply(parentWorld);
            tempMatrix.multiply(this.worldTransform);
            this.worldTransform = tempMatrix;
        }

        this.transformDirty = false;
    }
}