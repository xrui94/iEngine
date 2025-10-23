import type { EntityId } from './Entity';
import { LightECS } from './components/LightECS';
import { ComponentTypeRegistry, ComponentKey } from './ComponentType';

/**
 * ECS数据容器：只存储原始的组件
 */
export class WorldECS {
    private entities: Set<EntityId> = new Set();
    private comps: Map<string, Map<EntityId, any>> = new Map();
    // 位掩码：每个实体对应的组件掩码
    private entityMask: Map<EntityId, number> = new Map();
    private registry = ComponentTypeRegistry.instance();

    createEntity(id: EntityId): void {
        this.entities.add(id);
        this.entityMask.set(id, 0);
    }

    destroyEntity(id: EntityId): void {
        this.entities.delete(id);
        this.entityMask.delete(id);
        for (const store of this.comps.values()) {
            store.delete(id);
        }
    }

    addComponent<T>(id: EntityId, key: ComponentKey, comp: T): void {
        let store = this.comps.get(key);
        if (!store) {
            store = new Map<EntityId, T>();
            this.comps.set(key, store);
        }
        store.set(id, comp);
        // 更新实体掩码
        const bit = this.registry.getMask(key);
        const prev = this.entityMask.get(id) || 0;
        this.entityMask.set(id, prev | bit);
    }

    removeComponent(id: EntityId, key: ComponentKey): void {
        const store = this.comps.get(key);
        store?.delete(id);
        // 清除实体掩码对应位
        const bit = this.registry.getMask(key);
        const prev = this.entityMask.get(id) || 0;
        this.entityMask.set(id, prev & ~bit);
    }

    getComponent<T>(id: EntityId, key: ComponentKey): T | undefined {
        const store = this.comps.get(key) as Map<EntityId, T> | undefined;
        return store?.get(id);
    }

    hasComponent(id: EntityId, key: ComponentKey): boolean {
        const store = this.comps.get(key);
        return !!store?.has(id);
    }

    /**
     * 位掩码组合查询：
     * - all: 必须具备的组件集合（AND）
     * - any: 至少具备一个的组件集合（OR）
     * - none: 必须不具备的组件集合（NOT）
     */
    queryMask(criteria: { all?: ComponentKey[]; any?: ComponentKey[]; none?: ComponentKey[] }): EntityId[] {
        const allMask = criteria.all?.length ? this.registry.getCombinedMask(criteria.all) : 0;
        const anyMask = criteria.any?.length ? this.registry.getCombinedMask(criteria.any) : 0;
        const noneMask = criteria.none?.length ? this.registry.getCombinedMask(criteria.none) : 0;

        const out: EntityId[] = [];
        for (const id of this.entities) {
            const mask = this.entityMask.get(id) || 0;
            // all: (mask & allMask) === allMask
            if (allMask && ((mask & allMask) !== allMask)) continue;
            // any: anyMask === 0 或者 (mask & anyMask) !== 0
            if (anyMask && ((mask & anyMask) === 0)) continue;
            // none: (mask & noneMask) === 0
            if (noneMask && ((mask & noneMask) !== 0)) continue;
            out.push(id);
        }
        return out;
    }

    getLights(): LightECS[] {
        const store = this.comps.get('LightECS') as Map<EntityId, LightECS> | undefined;
        if (!store) return [];
        return Array.from(store.values());
    }
}