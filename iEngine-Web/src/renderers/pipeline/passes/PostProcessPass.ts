import type { RenderPass, RenderPassContext } from '../RenderGraph';

export class PostProcessPass implements RenderPass {
    name = 'PostProcessPass';
    enabled = true;

    execute(_ctx: RenderPassContext): void {
        // 雏形阶段暂不做任何事情
        // 未来可接入屏幕后处理（如色调映射、FXAA、Bloom等）
    }
}