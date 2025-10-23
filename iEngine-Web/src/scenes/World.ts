import { Scene } from './Scene';
import { Node } from '../core/Node';
import { Component } from '../core/Component';
import { RenderableComponent } from '../renderers/RenderableComponent';
import { LightComponent } from '../lights/LightComponent';
import { Renderable } from '../renderers/Renderable';

import type { Light } from '../lights/Light';

/**
 * World 负责维护场景中可被系统迭代的数据视图。
 * 采用轻量扫描策略，避免强耦合到 Node/Scene 的增删钩子。
 */
export class World {
    private renderables: Renderable[] = [];
    private lights: Light[] = [];

    /**
     * 扫描场景，构建当前帧的数据视图。
     * 目前侧重 RenderableComponent 与 LightComponent。
     */
    scanScene(scene: Scene): void {
        this.renderables = [];
        this.lights = [];

        const roots = scene.getEntities();
        for (const node of roots) {
            this.visitNode(node);
        }
    }

    private visitNode(node: Node): void {
        // 收集渲染组件
        const rc = node.getComponent(RenderableComponent);
        if (rc) {
            this.renderables.push({
                mesh: rc.mesh,
                material: rc.material,
                worldTransform: node.getWorldTransform(),
                layer: rc.layer,
            });
        }

        // 收集光照组件
        const lc = node.getComponent(LightComponent);
        if (lc) {
            this.lights.push(lc.light);
        }

        // 递归子节点
        for (const child of node.getChildren()) {
            this.visitNode(child);
        }
    }

    getRenderables(): Renderable[] {
        // 返回副本，避免外部修改内部数组
        return this.renderables.slice();
    }

    getLights(): Light[] {
        return this.lights.slice();
    }
}