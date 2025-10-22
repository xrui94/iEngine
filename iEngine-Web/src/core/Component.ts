import { Node } from './Node';

export abstract class Component {
    // 仅用于类型识别（可选）
    abstract readonly type: string;
    
    // 组件所属的节点
    private node: Node | null = null;
    
    // 组件是否启用
    public enabled: boolean = true;
    
    // 设置组件所属的节点
    setNode(node: Node | null): void {
        this.node = node;
    }
    
    // 获取组件所属的节点
    getNode(): Node | null {
        return this.node;
    }
    
    // 组件生命周期方法
    onAttach(): void {
        // 组件被添加到节点时调用
    }
    
    onDetach(): void {
        // 组件从节点移除时调用
    }
    
    // 组件启用时调用
    onEnable(): void {
        // 组件启用时的逻辑
    }
    
    // 组件禁用时调用
    onDisable(): void {
        // 组件禁用时的逻辑
    }
    
    onUpdate(deltaTime: number): void {
        // 组件更新逻辑
    }
    
    // 组件销毁时调用
    onDestroy(): void {
        // 组件销毁时的清理逻辑
    }
}