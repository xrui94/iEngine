import { Scene } from '../scenes/Scene'

import type { Renderable } from './Renderable'
import type { Light } from '../lights/Light'
import type { Context } from './Context'

export type RendererType = 'webgl' | 'webgpu'
export type GraphicsAPI = 'webgl1' | 'webgl2' | 'webgpu'

export abstract class Renderer {
    static isRendererType(value: any): value is RendererType {
        return value === 'webgl' || value === 'webgpu'
    }

    static isGraphicsAPI(value: any): value is GraphicsAPI {
        return value === 'webgl1' || value === 'webgl2' || value === 'webgpu'
    }

    abstract isInitialized(): boolean

    abstract setViewport(viewport?: { x: number; y: number; width: number; height: number }): void;
   
    abstract setClearColor(color?: [number, number, number, number]): void;

    // 渲染方法，只接收场景参数
    abstract render(scene: Scene, options?: { clearCanvas?: boolean }): void

    abstract resize(): void

    abstract clear(): void

    // 统一聚合：Renderable（含去重）
    protected aggregateRenderables(scene: Scene): Renderable[] {
        const merged: Renderable[] = scene.getRenderables();

        const final: Renderable[] = [];
        const dedupe = new Map<any, Map<any, Set<number>>>();
        for (const r of merged) {
            let matMap = dedupe.get(r.mesh);
            if (!matMap) { matMap = new Map<any, Set<number>>(); dedupe.set(r.mesh, matMap); }
            let layerSet = matMap.get(r.material);
            if (!layerSet) { layerSet = new Set<number>(); matMap.set(r.material, layerSet); }
            const layerId = (r as any).layer ?? 0;
            if (layerSet.has(layerId)) continue;
            layerSet.add(layerId);
            final.push(r);
        }
        return final;
    }

    // 统一聚合：Lights（含去重）
    protected aggregateLights(scene: Scene): Light[] {
        const mergedLights: Light[] = scene.getLights();

        const finalLights: Light[] = [];
        const lightSeen = new Set<string>();
        const lightKey = (l: Light) => {
            const anyL: any = l as any;
            const pos = anyL.position ? `${anyL.position.x},${anyL.position.y},${anyL.position.z}` : '';
            const dir = anyL.direction ? `${anyL.direction.x},${anyL.direction.y},${anyL.direction.z}` : '';
            const range = anyL.range ?? '';
            const angle = anyL.angle ?? '';
            const castShadow = anyL.castShadow ?? '';
            const shadowBias = anyL.shadowBias ?? '';
            const color = (l as any).color?.toArray ? (l as any).color.toArray().join(',') : `${(l as any)._color?.toArray?.().join(',')}`;
            return `${(l as any).type}|${color}|${(l as any).intensity}|${pos}|${dir}|${range}|${angle}|${castShadow}|${shadowBias}`;
        };
        for (const l of mergedLights) {
            const key = lightKey(l);
            if (!lightSeen.has(key)) { lightSeen.add(key); finalLights.push(l); }
        }
        return finalLights;
    }

    // 清理 GPU 资源(后边需要实现这个接口)
    // abstract dispose(): void;
}