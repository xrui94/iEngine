import { Matrix4, Vector3 } from '../../math';
import type { EntityId } from '../Entity';

export class Transform {
    parent: EntityId | null = null;
    position: Vector3 = new Vector3();
    rotation: Vector3 = new Vector3();
    scale: Vector3 = new Vector3(1, 1, 1);
    worldMatrix: Matrix4 = new Matrix4();
    dirty = true;
}