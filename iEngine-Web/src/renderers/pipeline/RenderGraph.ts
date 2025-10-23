import type { Scene } from '../../scenes/Scene';
import type { Renderer } from '../Renderer';

export interface RenderPassContext {
    renderer: Renderer;
    scene: Scene;
}

export interface RenderPass {
    name: string;
    enabled: boolean;
    execute(ctx: RenderPassContext): void;
}

export class RenderGraph {
    private passes: RenderPass[] = [];

    addPass(pass: RenderPass): void {
        this.passes.push(pass);
    }

    clear(): void {
        this.passes = [];
    }

    execute(renderer: Renderer, scene: Scene): void {
        const ctx: RenderPassContext = { renderer, scene };
        for (const p of this.passes) {
            if (!p.enabled) continue;
            p.execute(ctx);
        }
    }
}