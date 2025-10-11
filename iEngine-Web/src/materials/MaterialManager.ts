import { BaseMaterial } from "./BaseMaterial";
import { PhongMaterial } from "./PhongMaterial";
import { PbrMaterial } from "./PbrMaterial";

import type { Material } from "./Material";
import type { GraphicsAPI } from "../renderers/Renderer";

export class MaterialManager {
    private static factories = new Map<string, () => Material>();
    private static cache = new Map<string, Material>();

    static register(name: string, factory: () => Material) {
        this.factories.set(name, factory);
    }

    static get(name: string): Material {
        if (!this.cache.has(name)) {
            const factory = this.factories.get(name);
            if (!factory) throw new Error(`Material ${name} not registered.`);
            const mat = factory();
            this.cache.set(name, mat);
        }
        return this.cache.get(name)!;
    }

    static clear() {
        this.cache.clear();
    }

    /** 可选：在引擎 start() 中调用，根据后端预热所有 Shader 变体 */
    static warmUpShaders(api: GraphicsAPI) {
        for (const name of this.factories.keys()) {
            const material = this.get(name);
            // material?.initShader(api); // 假设你的 Material 基类有此方法
        }
    }
}

/** 可选：批量注册常用材质 */
export function registerBuiltInMaterials() {
    MaterialManager.register('base', () => new BaseMaterial());
    // MaterialManager.register('wireframe', () => new WireframeMaterial());
    MaterialManager.register('phong', () => new PhongMaterial());
    MaterialManager.register('pbr', () => new PbrMaterial());
}
