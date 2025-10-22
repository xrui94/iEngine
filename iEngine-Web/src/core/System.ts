export abstract class System {
    // 系统优先级，数值越小优先级越高
    public priority: number = 0;
    
    // 系统名称
    public name: string;
    
    constructor(name: string) {
        this.name = name;
    }
    
    // 系统初始化
    abstract initialize(): void;
    
    // 系统更新
    abstract update(deltaTime: number): void;
    
    // 系统销毁
    abstract destroy(): void;
}