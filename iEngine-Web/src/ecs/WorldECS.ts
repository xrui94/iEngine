import type { EntityId } from './Entity';
import { LightECS } from './components/LightECS';

/**
 * ECS数据容器：只存储原始的组件
 */
export class WorldECS {
    private entities: Set<EntityId> = new Set();
    private comps: Map<string, Map<EntityId, any>> = new Map();

    createEntity(id: EntityId): void {
        this.entities.add(id);
    }

    destroyEntity(id: EntityId): void {
        this.entities.delete(id);
        for (const store of this.comps.values()) {
            store.delete(id);
        }
    }

    addComponent<T>(id: EntityId, key: string, comp: T): void {
        let store = this.comps.get(key);
        if (!store) {
            store = new Map<EntityId, T>();
            this.comps.set(key, store);
        }
        store.set(id, comp);
    }

    removeComponent(id: EntityId, key: string): void {
        const store = this.comps.get(key);
        store?.delete(id);
    }

    getComponent<T>(id: EntityId, key: string): T | undefined {
        const store = this.comps.get(key) as Map<EntityId, T> | undefined;
        return store?.get(id);
    }

    hasComponent(id: EntityId, key: string): boolean {
        const store = this.comps.get(key);
        return !!store?.has(id);
    }

    query(all: string[]): EntityId[] {
        const result: EntityId[] = [];
        for (const id of this.entities) {
            let ok = true;
            for (const key of all) {
                if (!this.hasComponent(id, key)) { ok = false; break; }
            }
            if (ok) result.push(id);
        }
        return result;
    }

    getLights(): LightECS[] {
        const store = this.comps.get('LightECS') as Map<EntityId, LightECS> | undefined;
        if (!store) return [];
        return Array.from(store.values());
    }
}