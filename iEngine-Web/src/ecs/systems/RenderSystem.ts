import { SystemECS } from '../System';
import type { WorldECS } from '../WorldECS';
import { Transform } from '../components/TransformECS';
import { MeshECS } from '../components/MeshECS';
import { MaterialECS } from '../components/MaterialECS';
import { RenderLayerECS } from '../components/RenderLayerECS';
import type { Renderable } from '../../renderers/Renderable';
import { RenderLayerID } from '../../renderers/RenderLayer';

export class RenderSystem extends SystemECS {
    private cache: Renderable[] = [];

    constructor() {
        super('RenderSystem');
        this.priority = 30; // 在 Transform 和 Light 系统之后
    }

    update(world: WorldECS, _deltaTime: number): void {
        const ids = world.queryMask({ all: ['Transform', 'MeshECS', 'MaterialECS'], none: ['Disabled', 'Hidden'] });
        const out: Renderable[] = [];
        for (const id of ids) {
            const t = world.getComponent<Transform>(id, 'Transform');
            const m = world.getComponent<MeshECS>(id, 'MeshECS');
            const mat = world.getComponent<MaterialECS>(id, 'MaterialECS');
            const layer = world.getComponent<RenderLayerECS>(id, 'RenderLayerECS');
            if (!t || !m || !mat) continue;
            // 防御性过滤：丢弃无效网格或材质
            if ((m as any).mesh == null || (mat as any).material == null) continue;
            out.push({
                mesh: m.mesh,
                material: mat.material,
                worldTransform: t.worldMatrix,
                layer: layer?.layer ?? RenderLayerID.Opaque,
            });
        }
        this.cache = out;
    }

    getRenderables(): Renderable[] {
        return this.cache.slice();
    }
}