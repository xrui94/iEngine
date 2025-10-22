import { System } from './System';

export class SystemManager {
    private systems: System[] = [];
    
    // 添加系统
    addSystem(system: System): void {
        // 检查系统是否已存在
        if (this.systems.some(s => s.name === system.name)) {
            console.warn(`System "${system.name}" already exists`);
            return;
        }
        
        // 按优先级排序插入
        const index = this.systems.findIndex(s => s.priority > system.priority);
        if (index === -1) {
            this.systems.push(system);
        } else {
            this.systems.splice(index, 0, system);
        }
        
        // 初始化系统
        system.initialize();
    }
    
    // 移除系统
    removeSystem(systemName: string): boolean {
        const index = this.systems.findIndex(s => s.name === systemName);
        if (index !== -1) {
            const system = this.systems[index];
            system.destroy();
            this.systems.splice(index, 1);
            return true;
        }
        return false;
    }
    
    // 获取系统
    getSystem<T extends System>(systemName: string): T | undefined {
        return this.systems.find(s => s.name === systemName) as T | undefined;
    }
    
    // 更新所有系统
    update(deltaTime: number): void {
        for (const system of this.systems) {
            system.update(deltaTime);
        }
    }
    
    // 销毁所有系统
    destroy(): void {
        // 反向销毁（优先级高的先销毁）
        for (let i = this.systems.length - 1; i >= 0; i--) {
            this.systems[i].destroy();
        }
        this.systems = [];
    }
}