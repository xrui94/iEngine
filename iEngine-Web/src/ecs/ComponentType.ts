export type ComponentKey = string;

/**
 * 组件类型注册器：为每个组件类型分配唯一 bit 位。
 * 目前使用 32-bit 掩码，可根据需要扩展为 BigInt。
 */
export class ComponentTypeRegistry {
    private static _instance: ComponentTypeRegistry | null = null;
    private keyToBit: Map<ComponentKey, number> = new Map();
    private nextBit: number = 0;

    static instance(): ComponentTypeRegistry {
        if (!this._instance) this._instance = new ComponentTypeRegistry();
        return this._instance;
    }

    /**
     * 注册组件类型，返回对应位掩码；若已注册则返回现有掩码。
     */
    register(key: ComponentKey): number {
        const exist = this.keyToBit.get(key);
        if (exist !== undefined) return (1 << exist);
        const bitIndex = this.nextBit++;
        this.keyToBit.set(key, bitIndex);
        return (1 << bitIndex);
    }

    /** 获取组件类型对应的位掩码，未注册则自动注册。 */
    getMask(key: ComponentKey): number {
        return this.register(key);
    }

    /** 获取多个组件类型的合并掩码（任意或全部匹配时可用）。 */
    getCombinedMask(keys: ComponentKey[]): number {
        let mask = 0;
        for (const k of keys) mask |= this.getMask(k);
        return mask;
    }
}