import { SystemECS } from '../System';
import type { WorldECS } from '../WorldECS';
import { Transform } from '../components/TransformECS';
import { LightECS } from '../components/LightECS';
import { Vector3 } from '../../math/Vector3';
import { AmbientLight } from '../../lights/AmbientLight';
import { DirectionalLight } from '../../lights/DirectionalLight';
import { PointLight } from '../../lights/PointLight';
import { SpotLight } from '../../lights/SpotLight';

function rotateVectorByEuler(v: Vector3, rot: Vector3): Vector3 {
    // X
    const cx = Math.cos(rot.x), sx = Math.sin(rot.x);
    let vy = v.y * cx - v.z * sx;
    let vz = v.y * sx + v.z * cx;
    let vx = v.x;
    // Y
    const cy = Math.cos(rot.y), sy = Math.sin(rot.y);
    const vx2 = vx * cy + vz * sy;
    const vz2 = -vx * sy + vz * cy;
    vy = vy; vx = vx2; vz = vz2;
    // Z
    const cz = Math.cos(rot.z), sz = Math.sin(rot.z);
    const vx3 = vx * cz - vy * sz;
    const vy3 = vx * sz + vy * cz;
    vx = vx3; vy = vy3;
    return new Vector3(vx, vy, vz).normalize();
}

export class LightSystem extends SystemECS {
    constructor() {
        super('LightSystem');
        this.priority = 20;
    }

    update(world: WorldECS, _deltaTime: number): void {
        const ids = world.queryMask({ all: ['Transform', 'LightECS'], none: ['Disabled', 'Hidden'] });
        for (const id of ids) {
            const t = world.getComponent<Transform>(id, 'Transform');
            const le = world.getComponent<LightECS>(id, 'LightECS');
            if (!t || !le) continue;
            const e = t.worldMatrix.elements;
            const pos = new Vector3(e[12], e[13], e[14]);
            const light = le.light;
            if (light instanceof AmbientLight) {
                // 环境光无需位姿
                continue;
            } else if (light instanceof PointLight) {
                light.position.copy(pos);
            } else if (light instanceof SpotLight) {
                light.position.copy(pos);
                // 方向按欧拉旋转应用到默认向下方向
                const dir = rotateVectorByEuler(new Vector3(0, -1, 0), t.rotation);
                light.direction.copy(dir);
            } else if (light instanceof DirectionalLight) {
                light.position.copy(pos);
                const dir = rotateVectorByEuler(new Vector3(0, -1, 0), t.rotation);
                light.direction.copy(dir);
            } else {
                // 其它类型：保持最小更新
                (light as any).position?.copy?.(pos);
            }
        }
    }
}