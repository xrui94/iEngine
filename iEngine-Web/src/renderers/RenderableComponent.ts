import { Component } from '../core/Component';
import { RenderLayerID } from './RenderLayer';

import type { Mesh } from '../core/Mesh';
import type { Material } from '../materials/Material';

export class RenderableComponent extends Component {
    readonly type = 'RenderableComponent';

    constructor(
        public mesh: Mesh,
        public material: Material,
        public layer: RenderLayerID = RenderLayerID.Opaque
    ) {
        super();
    }
    
    onUpdate(deltaTime: number): void {
        // 渲染组件更新逻辑
        // 例如：动画更新、材质参数更新等
    }
    
    // 获取世界变换矩阵
    getWorldTransform(): Float32Array | null {
        const node = this.getNode();
        if (node) {
            const worldMatrix = node.getWorldTransform();
            return worldMatrix.elements;
        }
        return null;
    }
}