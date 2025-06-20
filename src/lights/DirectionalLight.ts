import { Color } from '../materials/Color';
import { Vector3 } from '../math/Vector3';
import { Light } from './Light';

export type DirectionalLightOptions = {
    position: Vector3;      // 光源位置
    direction: Vector3;     // 光照方向
    castShadow: boolean;    // 灯光是否投射阴影
    shadowBias: number;     // 阴影偏移量
    color: Color | [number, number, number],
    intensity: number
}

export class DirectionalLight extends Light {
    public position: Vector3;           // 光源位置
    public direction: Vector3;          // 光照方向
    public castShadow: boolean = false;  // 灯光是否投射阴影
    public shadowBias: number = 0.0;    // 阴影偏移量

    constructor({
        color = Color.white,
        intensity = 1.0,
        position = new Vector3(0, 10, 0),
        direction = new Vector3(0, -1, 0)
    } : DirectionalLightOptions) {
        super({
            type: 'directional',
            color,
            intensity
        });
        this.position = position;
        this.direction = direction;
    }
}