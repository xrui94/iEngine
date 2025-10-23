import { SystemECS } from './System';
import type { WorldECS } from './WorldECS';

export class SystemManagerECS {
    private systems: SystemECS[] = [];

    addSystem(system: SystemECS): void {
        this.systems.push(system);
        this.systems.sort((a, b) => a.priority - b.priority);
        system.initialize();
    }

    update(world: WorldECS, deltaTime: number): void {
        for (const sys of this.systems) {
            sys.update(world, deltaTime);
        }
    }

    clear(): void {
        for (const sys of this.systems) {
            sys.destroy();
        }
        this.systems = [];
    }
}