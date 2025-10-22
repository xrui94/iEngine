import { System } from '../core/System';
import { Node } from '../core/Node';
import { RenderableComponent } from './RenderableComponent';
import { Renderable } from './Renderable';
import { RenderLayer, RenderLayerID } from './RenderLayer';
import { Renderer } from './Renderer';
import { Scene } from '../scenes/Scene';

export class RenderSystem extends System {
    private layers: Map<RenderLayerID, RenderLayer> = new Map();
    private renderer: Renderer | null = null;
    private scene: Scene | null = null;
    
    constructor() {
        super('RenderSystem');
        this.priority = 100; // 渲染系统优先级较低
    }
    
    initialize(): void {
        // 初始化渲染层
        this.initializeLayers();
    }
    
    // 设置渲染器
    setRenderer(renderer: Renderer): void {
        this.renderer = renderer;
    }
    
    // 设置场景
    setScene(scene: Scene): void {
        this.scene = scene;
    }
    
    private initializeLayers(): void {
        // 创建默认渲染层
        const opaqueLayer: RenderLayer = {
            id: RenderLayerID.Opaque,
            renderQueue: [],
            needsSorting: false
        };
        
        const transparentLayer: RenderLayer = {
            id: RenderLayerID.Transparent,
            renderQueue: [],
            needsSorting: true
        };
        
        const uiLayer: RenderLayer = {
            id: RenderLayerID.UI,
            renderQueue: [],
            needsSorting: false
        };
        
        this.layers.set(RenderLayerID.Opaque, opaqueLayer);
        this.layers.set(RenderLayerID.Transparent, transparentLayer);
        this.layers.set(RenderLayerID.UI, uiLayer);
    }
    
    update(deltaTime: number): void {
        // 收集所有可渲染对象
        this.collectRenderables();
        
        // 排序渲染队列
        this.sortRenderQueues();
        
        // 执行渲染
        this.executeRender();
    }
    
    private collectRenderables(): void {
        // 清空所有渲染队列
        for (const layer of this.layers.values()) {
            layer.renderQueue = [];
        }
        
        // 如果没有设置场景或渲染器，直接返回
        if (!this.scene || !this.renderer) {
            return;
        }
        
        // 从场景中收集所有可渲染对象
        const renderables: Renderable[] = [];
        this.scene.collectRenderables(renderables);
        
        // 将收集到的可渲染对象按层分类
        for (const renderable of renderables) {
            const layer = this.layers.get(renderable.layer);
            if (layer) {
                layer.renderQueue.push(renderable);
            }
        }
    }
    
    private sortRenderQueues(): void {
        for (const layer of this.layers.values()) {
            if (layer.needsSorting) {
                // 按照距离相机远近排序（透明物体从后往前渲染）
                layer.renderQueue.sort((a, b) => {
                    // 简单的排序逻辑，实际项目中可能需要更复杂的实现
                    return 0;
                });
            }
        }
    }
    
    private executeRender(): void {
        // 按照层顺序执行渲染
        if (!this.renderer || !this.scene) {
            return;
        }
        
        // 调用渲染器的渲染方法，只传递场景
        this.renderer.render(this.scene);
    }
    
    destroy(): void {
        // 清理资源
        this.layers.clear();
        this.renderer = null;
        this.scene = null;
    }
}