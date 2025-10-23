import type { RenderPass, RenderPassContext } from '../RenderGraph';

export class ForwardPass implements RenderPass {
    name = 'ForwardPass';
    enabled = true;

    execute(ctx: RenderPassContext): void {
        // 在最简版本中，直接调用底层渲染器的 render
        // 未来可在此设置必要的渲染状态（如深度、混合等）
        ctx.renderer.render(ctx.scene, { clearCanvas: true });
    }
}