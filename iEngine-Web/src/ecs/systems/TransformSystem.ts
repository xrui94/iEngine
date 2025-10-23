import { SystemECS } from '../System';
import type { WorldECS } from '../WorldECS';
import { Transform } from '../components/TransformECS';
import { Matrix4 } from '../../math/Matrix4';
import { Vector3 } from '../../math/Vector3';
import type { EntityId } from '../Entity';

function composeEuler(position: Vector3, rotation: Vector3, scale: Vector3): Matrix4 {
    const T = Matrix4.fromTranslation(position);
    const Rx = Matrix4.fromRotation({ x: 1, y: 0, z: 0 }, rotation.x);
    const Ry = Matrix4.fromRotation({ x: 0, y: 1, z: 0 }, rotation.y);
    const Rz = Matrix4.fromRotation({ x: 0, y: 0, z: 1 }, rotation.z);
    const S = Matrix4.fromScaling(scale);
    const world = Matrix4.identity();
    world.multiply(T).multiply(Rx).multiply(Ry).multiply(Rz).multiply(S);
    return world;
}

export class TransformSystem extends SystemECS {
    constructor() {
        super('TransformSystem');
        this.priority = 10;
    }

    update(world: WorldECS, _deltaTime: number): void {
        const ids = world.query(['Transform']);
        for (const id of ids) {
            const t = world.getComponent<Transform>(id, 'Transform');
            if (!t || !t.dirty) continue;
            const local = composeEuler(t.position, t.rotation, t.scale);
            if (t.parent && world.hasComponent(t.parent, 'Transform')) {
                const p = world.getComponent<Transform>(t.parent, 'Transform');
                if (p) {
                    const wm = new Matrix4([...p.worldMatrix.elements]);
                    wm.multiply(local);
                    t.worldMatrix = wm;
                } else {
                    t.worldMatrix = local;
                }
            } else {
                t.worldMatrix = local;
            }
            t.dirty = false;
        }
    }
}