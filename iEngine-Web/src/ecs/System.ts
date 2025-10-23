import type { WorldECS } from './WorldECS';

export abstract class SystemECS {
    name: string;
    priority = 0;
    constructor(name: string) {
        this.name = name;
    }
    initialize(): void {}
    abstract update(world: WorldECS, deltaTime: number): void;
    destroy(): void {}
}